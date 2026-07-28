#pragma once

#include "AMSimPhase6Types.h"

class FArchive;

namespace AMSim
{
	void SerializePhase6State(FArchive& Archive, FPhase6State& State);
}
