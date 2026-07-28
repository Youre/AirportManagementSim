#pragma once

#include "AMSimPhase5Types.h"

namespace AMSim
{
	struct FPhase5ViewState
	{
		uint64 Revision = 0;
		FString Status;
		FString CargoSummary;
		FString ShipmentHeadline;
		FString ShipmentDetail;
		FString Flow;
		FString Warehouse;
		FString Providers;
		FString Events;
		FString Progression;
		FString Cause;
		FString Remedy;
		TArray<FString> ContractCards;
		TArray<FString> ZoneCards;
		TArray<FString> ProviderCards;
		TArray<FString> EventCards;
		TArray<FString> CapabilityCards;
		bool bCanInitialize = false;
		bool bCanAcceptCargo = false;
		bool bCanAdvanceCargo = false;
		bool bCanAcceptProvider = false;
		bool bCanAcceptEvent = false;
		bool bCanAdvanceEvent = false;
		bool bCanRecover = false;
	};

	AMSIMUI_API FPhase5ViewState MakePhase5ViewState(
		const FPhase5QuerySnapshot& Query,
		const FPhase5State& State);
}
