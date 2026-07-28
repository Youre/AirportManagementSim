#pragma once

#include "AMSimPhase5Types.h"

namespace AMSim
{
	struct FPhase6CapabilitySignals;

	using FPhase5ProgressionEmitter = TFunctionRef<void(
		EPhase5EventType,
		const FCommandId&,
		uint64,
		int64,
		const FString&)>;

	void RefreshAdvancedPathProgression(
		FPhase5State& State,
		int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State,
		FPhase5ProgressionEmitter Emit);

	void ApplyMajorCapabilitySignals(
		FPhase5State& State,
		const FPhase6CapabilitySignals& Signals,
		int64 CurrentGameMilliseconds,
		FPhase5ProgressionEmitter Emit);
}
