#include "AMSimPhase2ViewState.h"
#include "AMSimPhase2Fixture.h"

namespace AMSim
{
	FPhase2ViewState MakePhase2ViewState(
		const FPhase2QuerySnapshot& Query,
		const FPhase2State& State,
		const FPhase1State& Phase1State)
	{
		FPhase2ViewState View;
		View.Revision = Query.Revision;
		View.Status = Query.PrimaryStatus;
		View.Operations = FString::Printf(
			TEXT("%d ACTIVE · %d DONE · RWY %s"),
			Query.ScheduledFlightCount,
			Query.CompletedFlightCount,
			*Query.ActiveRunwayDirection.ToString());
		View.Weather = FString::Printf(
			TEXT("%s · %d KT"),
			*WeatherCategoryDisplayName(Query.WeatherCategory).ToUpper(),
			State.CurrentWeather.WindSpeedKnots);
		View.Staff = FString::Printf(
			TEXT("%d/%d TEAMS · %d%% LOAD · %d/%d VEHICLES"),
			Query.BusyTeamCount,
			State.Teams.Num(),
			Query.AverageWorkloadPercent,
			Query.BusyVehicleCount,
			State.Vehicles.Num());
		View.Tenant = FString::Printf(
			TEXT("%s · %d RATING"),
			*SpecializationDisplayName(Query.Specialization).ToUpper(),
			Query.AverageRating);
		View.Economy = FString::Printf(
			TEXT("+%lld / -%lld CR"),
			State.TotalPhase2RevenueCredits,
			State.TotalPhase2CostCredits);
		View.Cause = Query.Cause;
		View.Remedy = Query.Remedy;
		const bool bExpansionClosureActive =
			State.Expansion.Stage != EExpansionStage::None &&
			State.Expansion.Stage != EExpansionStage::Operational &&
			(!State.Incident.Id.IsValid() ||
			 State.Incident.State == EIncidentState::Resolved);
		if (bExpansionClosureActive)
		{
			View.Cause = State.Expansion.ClosureSummary;
			View.Remedy =
				TEXT("UNAFFECTED: ACTIVE RWY + STAND A1 OPEN · WORK AREA: EAST TAXI SPUR");
		}
		View.bUnlocked =
			Phase1State.bInitialized &&
			Phase1State.bAirportOpen &&
			Phase1State.Flight.State == EFlightState::Completed;
		View.bInitialized = Query.bInitialized;
		if (View.bUnlocked && !View.bInitialized)
		{
			View.Status = TEXT("FIRST VISIT COMPLETE");
			View.Operations = TEXT("RECURRING FLIGHTS + EXPANSION READY");
			View.Weather = TEXT("START THE LIVING AIRPORT");
			View.Staff = TEXT("HIRE AND ASSIGN OPERATING TEAMS");
			View.Tenant = TEXT("CHOOSE GA, SCHOOL, OR CHARTER FIRST");
			View.Economy = TEXT("SHARED LEDGER CONTINUES");
		}
		View.bCanSelectIdentity = Query.bInitialized;
		View.bCanAcceptContract =
			State.SelectedSpecialization != EAirportSpecialization::Unselected &&
			State.Contracts.ContainsByPredicate(
				[&State](const FPhase2ContractRecord& Contract)
					{
						return Contract.Specialization == State.SelectedSpecialization &&
							!Contract.bAccepted;
					});
		View.bCanCancelContract = State.Contracts.ContainsByPredicate(
			[&State](const FPhase2ContractRecord& Contract)
				{
					if (Contract.Specialization != State.SelectedSpecialization ||
						!Contract.bAccepted)
					{
						return false;
					}
					return !State.Flights.ContainsByPredicate(
						[&Contract](const FPhase2FlightRecord& Flight)
							{
								return Flight.ContractId == Contract.Id &&
									Flight.State != EPhase2FlightState::Scheduled &&
									Flight.State != EPhase2FlightState::Completed &&
									Flight.State != EPhase2FlightState::Cancelled;
							});
				});
		View.bCanRescheduleFlight = State.Flights.ContainsByPredicate(
			[](const FPhase2FlightRecord& Flight)
				{
					return Flight.State == EPhase2FlightState::Scheduled;
				});
		View.bCanDispatchService = State.ServiceTasks.ContainsByPredicate(
			[](const FPhase2ServiceTaskRecord& Task)
				{
					return Task.State == EPhase2ServiceState::Queued ||
						Task.State == EPhase2ServiceState::Blocked;
				});
		View.bCanTow = State.Flights.ContainsByPredicate(
			[](const FPhase2FlightRecord& Flight)
				{
					return Flight.State == EPhase2FlightState::TaxiIn ||
						Flight.State == EPhase2FlightState::Turnaround ||
						Flight.State == EPhase2FlightState::ReadyToDepart;
				});
		View.bCanPurchaseParcel =
			!State.Parcels.IsEmpty() &&
			!State.Parcels[0].bOwned &&
			Phase1State.Credits >= State.Parcels[0].PurchaseCostCredits;
		View.bCanStartExpansion =
			!State.Parcels.IsEmpty() &&
			State.Parcels[0].bOwned &&
			State.Expansion.Stage == EExpansionStage::None;
		View.bCanRespondToIncident =
			State.Incident.Id.IsValid() &&
			State.Incident.State == EIncidentState::Reported;
		View.bCanRequestRecovery =
			Query.bInitialized &&
			Phase1State.Credits <= 500 &&
			State.RecoveryGrantCount < 2;
		return View;
	}
}
