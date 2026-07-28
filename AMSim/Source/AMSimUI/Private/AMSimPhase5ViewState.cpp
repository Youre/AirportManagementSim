#include "AMSimPhase5ViewState.h"

#include "AMSimPhase5Fixture.h"

namespace AMSim
{
	namespace
	{
		FString TenantStateName(const EProviderTenantState State)
		{
			switch (State)
			{
			case EProviderTenantState::Active: return TEXT("ACTIVE");
			case EProviderTenantState::Grace: return TEXT("RECOVERY");
			case EProviderTenantState::Suspended: return TEXT("SUSPENDED");
			default: return TEXT("OFFER");
			}
		}

		FString EventStateName(const ESpecialEventState State)
		{
			switch (State)
			{
			case ESpecialEventState::Preparing: return TEXT("PREP");
			case ESpecialEventState::Active: return TEXT("ACTIVE");
			case ESpecialEventState::PartialSuccess: return TEXT("PARTIAL");
			case ESpecialEventState::Cleanup: return TEXT("CLEANUP");
			case ESpecialEventState::Completed: return TEXT("COMPLETE");
			case ESpecialEventState::Cooldown: return TEXT("COOLDOWN");
			case ESpecialEventState::Declined: return TEXT("DECLINED");
			default: return TEXT("OFFER");
			}
		}
	}

	FPhase5ViewState MakePhase5ViewState(
		const FPhase5QuerySnapshot& Query,
		const FPhase5State& State)
	{
		FPhase5ViewState View;
		View.Revision = Query.Revision;
		View.Status = Query.PrimaryStatus;
		View.CargoSummary = Query.CargoSummary;
		View.Warehouse = Query.WarehouseSummary;
		View.Providers = Query.ProviderSummary;
		View.Events = Query.EventSummary;
		View.Progression = Query.ProgressionSummary;
		View.Cause = Query.Cause;
		View.Remedy = Query.Remedy;
		View.bCanInitialize = Query.bUnlocked && !Query.bInitialized;
		if (!Query.bInitialized)
		{
			View.ShipmentHeadline = TEXT("CARGO OPERATIONS");
			View.ShipmentDetail =
				TEXT("Initialize Phase 5 to open the cargo house.");
			View.Flow = TEXT("RECEIVE  →  SECURE  →  STORE  →  BUILD  →  LOAD");
			return View;
		}

		const FPhase5ShipmentRecord* Selected =
			State.Shipments.FindByPredicate(
				[](const FPhase5ShipmentRecord& Shipment)
				{
					return Shipment.State != ECargoState::Offered &&
						Shipment.State != ECargoState::Completed;
				});
		if (!Selected && !State.Shipments.IsEmpty())
		{
			Selected = &State.Shipments[0];
		}
		if (Selected)
		{
			View.ShipmentHeadline = FString::Printf(
				TEXT("%s · %s"),
				*CargoClassDisplayName(Selected->CargoClass).ToUpper(),
				*CargoFlowDisplayName(Selected->Flow).ToUpper());
			View.ShipmentDetail = FString::Printf(
				TEXT("%d PCS  ·  %lld KG  ·  %lld L\n%s"),
				Selected->Pieces,
				Selected->MassKilograms,
				Selected->VolumeLitres,
				*CargoStateDisplayName(Selected->State).ToUpper());
			View.Flow = FString::Printf(
				TEXT("ROAD  →  SECURITY  →  %s  →  AIRSIDE  →  AIRCRAFT"),
				*CargoStateDisplayName(Selected->State).ToUpper());
			View.bCanAdvanceCargo =
				Selected->State != ECargoState::Offered &&
				Selected->State != ECargoState::Completed &&
				!Selected->ActiveServiceTaskId.IsValid();
		}

		for (const FPhase5CargoContractRecord& Contract :
			State.CargoContracts)
		{
			View.ContractCards.Add(FString::Printf(
				TEXT("%s\n%s · %s   %lld CR"),
				*Contract.DisplayName.ToUpper(),
				*CargoClassDisplayName(Contract.CargoClass).ToUpper(),
				*CargoFlowDisplayName(Contract.Flow).ToUpper(),
				Contract.RewardCredits));
			View.bCanAcceptCargo |= !Contract.bAccepted;
		}
		for (const FPhase5WarehouseZoneRecord& Zone : State.WarehouseZones)
		{
			const int32 Percent = Zone.CapacityLitres > 0
				? static_cast<int32>(
					Zone.OccupiedLitres * 100 / Zone.CapacityLitres)
				: 0;
			View.ZoneCards.Add(FString::Printf(
				TEXT("%s\n%d%% · %lld / %lld L"),
				*Zone.ContentId.ToString()
					.Replace(TEXT("Warehouse."), TEXT(""))
					.ToUpper(),
				Percent,
				Zone.OccupiedLitres,
				Zone.CapacityLitres));
		}
		for (const FPhase5ProviderTenantRecord& Tenant :
			State.ProviderTenants)
		{
			View.ProviderCards.Add(FString::Printf(
				TEXT("%s\n%s · %d%% SAT"),
				*Tenant.DisplayName.ToUpper(),
				*TenantStateName(Tenant.State),
				Tenant.SatisfactionPercent));
			View.bCanAcceptProvider |=
				Tenant.State == EProviderTenantState::Offered;
			View.bCanRecover |=
				Tenant.State == EProviderTenantState::Grace ||
				Tenant.State == EProviderTenantState::Suspended;
		}
		for (const FPhase5SpecialEventRecord& Event : State.SpecialEvents)
		{
			const int32 CompletionPercent = Event.ExpectedDemand > 0
				? FMath::Clamp(
					Event.SatisfiedDemand * 100 / Event.ExpectedDemand,
					0,
					100)
				: 0;
			View.EventCards.Add(FString::Printf(
				TEXT("%s\n%s · %d%% SERVED"),
				*SpecialEventDisplayName(Event.Family).ToUpper(),
				*EventStateName(Event.State),
				CompletionPercent));
			View.bCanAcceptEvent |=
				Event.State == ESpecialEventState::Offered;
			View.bCanAdvanceEvent |=
				Event.State == ESpecialEventState::Preparing ||
				Event.State == ESpecialEventState::Active ||
				Event.State == ESpecialEventState::PartialSuccess ||
				Event.State == ESpecialEventState::Cleanup;
		}
		for (const FPhase5PathEvidenceRecord& Path : State.Paths)
		{
			View.CapabilityCards.Add(FString::Printf(
				TEXT("%s\n%s · %d AP · %d OPS"),
				*SpecializationPathDisplayName(Path.Path).ToUpper(),
				*CapabilityBandDisplayName(Path.Band).ToUpper(),
				Path.AirportPoints,
				Path.CompletedOperations));
		}
		return View;
	}
}
