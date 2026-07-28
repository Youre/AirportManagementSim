#pragma once

#include "AMSimPhase5Types.h"

class FArchive;

namespace AMSim
{
	void SerializePhase5State(
		FArchive& Archive,
		FPhase5State& State,
		uint32 SchemaVersion);
}
