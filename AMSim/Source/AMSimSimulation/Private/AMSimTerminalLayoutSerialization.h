#pragma once

#include "AMSimTerminalLayoutTypes.h"

class FArchive;

namespace AMSim
{
	void SerializeTerminalLayoutState(
		FArchive& Archive,
		FTerminalLayoutState& State);
}
