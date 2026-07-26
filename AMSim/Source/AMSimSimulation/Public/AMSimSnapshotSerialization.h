#pragma once

#include "AMSimSimulation.h"

namespace AMSim
{
	AMSIMSIMULATION_API bool SerializeSnapshot(const FSnapshot& Snapshot, TArray<uint8>& Output);
	AMSIMSIMULATION_API bool DeserializeSnapshot(const TArray<uint8>& Input, FSnapshot& Output);
}
