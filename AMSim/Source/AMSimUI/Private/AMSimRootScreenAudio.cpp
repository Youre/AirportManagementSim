#include "AMSimRootScreen.h"

#include "AMSimPhase4Types.h"
#include "AMSimPhase5Types.h"
#include "AMSimPhase6Types.h"
#include "AMSimUISoundSubsystem.h"

namespace
{
	int32 CountCompletedObjectives(const AMSim::FPhase1State& State)
	{
		int32 Result = 0;
		for (const AMSim::FObjectiveRecord& Objective : State.Objectives)
		{
			Result += Objective.bCompleted ? 1 : 0;
		}
		return Result;
	}
}

void UAMSimRootScreen::RefreshAudioFeedback(
	const AMSim::FPhase1QuerySnapshot& Phase1Query,
	const AMSim::FPhase1State& Phase1State,
	const AMSim::FPhase2QuerySnapshot& Phase2Query,
	const AMSim::FPhase4QuerySnapshot& Phase4Query,
	const AMSim::FPhase5QuerySnapshot& Phase5Query,
	const AMSim::FPhase6QuerySnapshot& Phase6Query)
{
	const uint8 OfferState = static_cast<uint8>(Phase1Query.OfferState);
	const uint8 Phase2Incident = static_cast<uint8>(Phase2Query.IncidentState);
	const uint8 Phase4Incident = static_cast<uint8>(Phase4Query.IncidentLifecycle);
	const uint8 Phase6Incident = static_cast<uint8>(Phase6Query.IncidentLifecycle);
	const int32 CompletedObjectives = CountCompletedObjectives(Phase1State);

	if (!bAudioFeedbackPrimed)
	{
		bAudioFeedbackPrimed = true;
		LastAudioOfferState = OfferState;
		LastAudioPhase2Incident = Phase2Incident;
		LastAudioPhase4Incident = Phase4Incident;
		LastAudioPhase6Incident = Phase6Incident;
		LastAudioCompletedObjectives = CompletedObjectives;
		LastAudioAdvancedPathCount = Phase5Query.AdvancedPathCount;
		LastAudioMajorPathCount = Phase6Query.MajorPathCount;
		LastAudioRecoveryGrantCount = Phase1State.RecoveryGrantCount;
		return;
	}

	TOptional<EAMSimUISound> Feedback;
	const auto Choose = [&Feedback](const EAMSimUISound Cue)
	{
		if (!Feedback.IsSet())
		{
			Feedback = Cue;
		}
	};

	// Safety feedback takes priority when several systems advance in one tick.
	if ((Phase2Incident != LastAudioPhase2Incident &&
		Phase2Query.IncidentState == AMSim::EIncidentState::Reported) ||
		(Phase4Incident != LastAudioPhase4Incident &&
		Phase4Query.IncidentLifecycle ==
			AMSim::EPhase4IncidentLifecycle::Alerted) ||
		(Phase6Incident != LastAudioPhase6Incident &&
		(Phase6Query.IncidentLifecycle ==
			AMSim::ESeriousIncidentLifecycle::Materialized ||
		 Phase6Query.IncidentLifecycle ==
			AMSim::ESeriousIncidentLifecycle::Alerted)))
	{
		Choose(EAMSimUISound::IncidentAlert);
	}
	else if ((Phase4Incident != LastAudioPhase4Incident &&
		Phase4Query.IncidentLifecycle ==
			AMSim::EPhase4IncidentLifecycle::Warned) ||
		(Phase6Incident != LastAudioPhase6Incident &&
		Phase6Query.IncidentLifecycle ==
			AMSim::ESeriousIncidentLifecycle::WarningIssued))
	{
		Choose(EAMSimUISound::WarningAttention);
	}
	else if (Phase5Query.AdvancedPathCount > LastAudioAdvancedPathCount ||
		Phase6Query.MajorPathCount > LastAudioMajorPathCount)
	{
		Choose(EAMSimUISound::CapabilityUnlock);
	}
	else if (CompletedObjectives > LastAudioCompletedObjectives)
	{
		Choose(EAMSimUISound::ObjectiveComplete);
	}
	else if (OfferState != LastAudioOfferState &&
		Phase1Query.OfferState == AMSim::EOfferState::Available)
	{
		Choose(EAMSimUISound::OfferAvailable);
	}
	else if (Phase1State.RecoveryGrantCount > LastAudioRecoveryGrantCount ||
		(Phase4Incident != LastAudioPhase4Incident &&
		Phase4Query.IncidentLifecycle ==
			AMSim::EPhase4IncidentLifecycle::Recovered) ||
		(Phase6Incident != LastAudioPhase6Incident &&
		Phase6Query.IncidentLifecycle ==
			AMSim::ESeriousIncidentLifecycle::Recovered))
	{
		Choose(EAMSimUISound::RecoverySuccess);
	}

	LastAudioOfferState = OfferState;
	LastAudioPhase2Incident = Phase2Incident;
	LastAudioPhase4Incident = Phase4Incident;
	LastAudioPhase6Incident = Phase6Incident;
	LastAudioCompletedObjectives = CompletedObjectives;
	LastAudioAdvancedPathCount = Phase5Query.AdvancedPathCount;
	LastAudioMajorPathCount = Phase6Query.MajorPathCount;
	LastAudioRecoveryGrantCount = Phase1State.RecoveryGrantCount;
	if (Feedback.IsSet())
	{
		AMSim::UIAudio::Play(this, Feedback.GetValue());
	}
}
