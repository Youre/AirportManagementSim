#pragma once

#include "AMSimSnapshotSerialization.h"
#include "Async/Future.h"

namespace AMSim
{
	struct FSaveMetadata
	{
		FString SlotId;
		FString PlayerLabel;
		FString AirportName;
		FString MapId = TEXT("Map.EmptyAirport");
		int64 CreatedUnixSeconds = 0;
		int64 LastPlayedUnixSeconds = 0;
		int64 GameTimeMilliseconds = 0;
		uint32 SaveSchema = SnapshotSchemaVersion;
		uint32 SimulationRulesVersion = 1;
		FString ContentManifestHash = TEXT("phase0-empty");
		FString LastResult = TEXT("Success");
	};

	struct FSaveResult
	{
		bool bSucceeded = false;
		FString Error;
	};

	class AMSIMGAMEPLAY_API FSaveStore
	{
	public:
		static TFuture<FSaveResult> WriteAsync(
			const FString& SlotDirectory,
			FSnapshot Snapshot,
			FSaveMetadata Metadata = {});
		static bool LoadCurrentOrBackup(const FString& SlotDirectory, FSnapshot& Output, bool& bUsedBackup);
		static bool LoadMetadata(const FString& SlotDirectory, FSaveMetadata& Output);
		static bool ValidateMetadata(const FSaveMetadata& Metadata, FString& Error);

		static FString CurrentPath(const FString& SlotDirectory);
		static FString BackupPath(const FString& SlotDirectory);
		static FString MetadataPath(const FString& SlotDirectory);
	};
}
