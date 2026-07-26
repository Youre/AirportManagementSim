#pragma once

#include "AMSimSimulationTypes.h"

namespace AMSim
{
	class AMSIMSIMULATION_API FDeterministicStream
	{
	public:
		explicit FDeterministicStream(uint64 InState = 1);

		uint64 NextUInt64();
		uint32 NextBounded(uint32 ExclusiveUpperBound);
		uint64 GetState() const { return State; }

		static uint64 SeedNamedStream(uint64 MasterSeed, const FString& StreamName);

	private:
		uint64 State;
	};

	class AMSIMSIMULATION_API FSimulationClock
	{
	public:
		void AdvanceSteps(int32 StepCount);
		bool Restore(int64 InGameTimeMilliseconds, uint64 InStepIndex);
		int64 GetGameTimeMilliseconds() const { return GameTimeMilliseconds; }
		uint64 GetStepIndex() const { return StepIndex; }

	private:
		int64 GameTimeMilliseconds = 0;
		uint64 StepIndex = 0;
	};
}
