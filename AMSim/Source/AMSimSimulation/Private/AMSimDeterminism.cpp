#include "AMSimDeterminism.h"

namespace AMSim
{
	namespace
	{
		constexpr uint64 NonZeroFallback = 0x9E3779B97F4A7C15ull;
	}

	FDeterministicStream::FDeterministicStream(const uint64 InState)
		: State(InState == 0 ? NonZeroFallback : InState)
	{
	}

	uint64 FDeterministicStream::NextUInt64()
	{
		State ^= State >> 12;
		State ^= State << 25;
		State ^= State >> 27;
		return State * 0x2545F4914F6CDD1Dull;
	}

	uint32 FDeterministicStream::NextBounded(const uint32 ExclusiveUpperBound)
	{
		return ExclusiveUpperBound == 0 ? 0 : static_cast<uint32>(NextUInt64() % ExclusiveUpperBound);
	}

	uint64 FDeterministicStream::SeedNamedStream(const uint64 MasterSeed, const FString& StreamName)
	{
		uint64 Hash = 1469598103934665603ull ^ MasterSeed;
		for (const TCHAR Character : StreamName)
		{
			Hash ^= static_cast<uint64>(Character);
			Hash *= 1099511628211ull;
		}
		return Hash == 0 ? NonZeroFallback : Hash;
	}

	void FSimulationClock::AdvanceSteps(const int32 StepCount)
	{
		check(StepCount >= 0);
		GameTimeMilliseconds += static_cast<int64>(StepCount) * FixedStepMilliseconds;
		StepIndex += static_cast<uint64>(StepCount);
	}

	bool FSimulationClock::Restore(const int64 InGameTimeMilliseconds, const uint64 InStepIndex)
	{
		if (InGameTimeMilliseconds < 0 ||
			InGameTimeMilliseconds % FixedStepMilliseconds != 0 ||
			static_cast<uint64>(InGameTimeMilliseconds / FixedStepMilliseconds) != InStepIndex)
		{
			return false;
		}
		GameTimeMilliseconds = InGameTimeMilliseconds;
		StepIndex = InStepIndex;
		return true;
	}
}
