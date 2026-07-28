#include "AMSimPhase4ViewState.h"

namespace AMSim
{
	FPhase4ViewState MakePhase4ViewState(
		const FPhase4QuerySnapshot& Query,
		const FPhase4State& State)
	{
		FPhase4ViewState View;
		View.Revision = Query.Revision;
		View.Status = Query.PrimaryStatus;
		View.Timetable = Query.TimetableSummary;
		View.SelectedFlight = Query.SelectedFlight;
		View.SelectedFlightDetail = Query.SelectedFlightDetail;
		View.Feasibility = Query.FeasibilitySummary;
		View.Connections = Query.ConnectionSummary;
		View.Border = Query.BorderSummary;
		View.Transport = Query.TransportSummary;
		View.Weather = Query.WeatherSummary;
		View.Forecast = Query.ForecastSummary;
		View.IncidentHeadline = Query.IncidentHeadline;
		View.IncidentCause = Query.IncidentCause;
		View.IncidentConsequence = Query.IncidentConsequence;
		View.IncidentRemedy = Query.IncidentRemedy;
		View.IncidentLifecycle = Query.IncidentLifecycleSummary;
		View.Caption = Query.Caption;
		View.Renewal = Query.RenewalSummary;
		View.bVisible = Query.bUnlocked || Query.bInitialized;
		View.bIncidentMode =
			Query.IncidentLifecycle >= EPhase4IncidentLifecycle::Alerted &&
			Query.IncidentLifecycle < EPhase4IncidentLifecycle::Recovered;
		View.bCanInitialize = Query.bUnlocked && !Query.bInitialized;
		View.bCanAcceptContract =
			Query.bInitialized &&
			!Query.bTimetablePublished &&
			Query.AcceptedContractCount < Query.ContractCount;
		View.bCanPublish =
			Query.bInitialized &&
			!Query.bTimetablePublished &&
			Query.ContractCount > 0 &&
			Query.AcceptedContractCount == Query.ContractCount;
		View.bCanReviewGateChange =
			Query.bTimetablePublished &&
			Query.GateChangeCount == 0 &&
			!View.bIncidentMode;
		View.bCanHold =
			View.bIncidentMode &&
			!State.Incident.bDeparturesHeld;
		View.bCanDivert =
			View.bIncidentMode &&
			!State.Incident.bArrivalsDiverted;
		View.bCanDispatchTow =
			View.bIncidentMode &&
			!State.Incident.bTowDispatched;
		View.bCanProtect =
			View.bIncidentMode &&
			State.Incident.bDeparturesHeld &&
			State.Incident.bArrivalsDiverted &&
			State.Incident.bTowDispatched &&
			!State.Incident.bAreaProtected;
		View.bCanReviewIncident =
			State.Incident.Lifecycle >=
				EPhase4IncidentLifecycle::AreaProtected &&
			!State.Incident.bReportReviewed;
		View.bCanRecover =
			State.Incident.bReportReviewed &&
			!State.Incident.bRecoveryApplied;
		View.bCanRenew = State.Renewals.ContainsByPredicate(
			[](const FPhase4RenewalRecord& Renewal)
				{
					return Renewal.State ==
						EPhase4RenewalState::Offered;
				});
		return View;
	}

	FPhase4FlightCardViewState MakePhase4FlightCardViewState(
		const FPhase4FlightRecord& Flight)
	{
		FPhase4FlightCardViewState View;
		if (Flight.bGateChanged && Flight.bOverrideRecorded)
		{
			View.Status = TEXT("LOCK");
			View.RiskLabel = TEXT("LOCK RISK ///");
			View.bWarning = true;
		}
		else if (Flight.bGateChanged)
		{
			View.Status = TEXT("R1 / BUS");
		}
		else if (Flight.bWeatherRestricted)
		{
			View.Status = TEXT("WX");
			View.RiskLabel = TEXT("WX CONFLICT ///");
			View.bWarning = true;
		}
		else if (Flight.bInternational)
		{
			View.Status = TEXT("INTL / READY");
		}
		else if (Flight.bCompleted)
		{
			View.Status = TEXT("COMPLETE");
			View.bPositive = true;
		}
		else
		{
			View.Status = TEXT("READY");
		}
		return View;
	}
}
