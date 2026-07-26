#include "AMSimSaveStore.h"
#include "Async/Async.h"
#include "HAL/FileManager.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace AMSim
{
	namespace
	{
		struct FSlotWriteState
		{
			FCriticalSection Mutex;
			bool bWorkerRunning = false;
			TOptional<FSnapshot> PendingSnapshot;
			TOptional<FSaveMetadata> PendingMetadata;
			TArray<TSharedPtr<TPromise<FSaveResult>, ESPMode::ThreadSafe>> PendingPromises;
		};

		FCriticalSection CoordinatorMutex;
		TMap<FString, TSharedPtr<FSlotWriteState, ESPMode::ThreadSafe>> SlotStates;

		bool SerializeMetadata(const FSaveMetadata& Metadata, FString& Output)
		{
			TSharedRef<FJsonObject> Object = MakeShared<FJsonObject>();
			Object->SetStringField(TEXT("slotId"), Metadata.SlotId);
			Object->SetStringField(TEXT("playerLabel"), Metadata.PlayerLabel);
			Object->SetStringField(TEXT("airportName"), Metadata.AirportName);
			Object->SetStringField(TEXT("mapId"), Metadata.MapId);
			Object->SetNumberField(TEXT("createdUnixSeconds"), Metadata.CreatedUnixSeconds);
			Object->SetNumberField(TEXT("lastPlayedUnixSeconds"), Metadata.LastPlayedUnixSeconds);
			Object->SetNumberField(TEXT("gameTimeMilliseconds"), Metadata.GameTimeMilliseconds);
			Object->SetNumberField(TEXT("saveSchema"), Metadata.SaveSchema);
			Object->SetNumberField(TEXT("simulationRulesVersion"), Metadata.SimulationRulesVersion);
			Object->SetStringField(TEXT("contentManifestHash"), Metadata.ContentManifestHash);
			Object->SetStringField(TEXT("lastResult"), Metadata.LastResult);
			const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
			return FJsonSerializer::Serialize(Object, Writer);
		}

		bool DeserializeMetadata(const FString& Input, FSaveMetadata& Output)
		{
			TSharedPtr<FJsonObject> Object;
			const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Input);
			if (!FJsonSerializer::Deserialize(Reader, Object) || !Object.IsValid())
			{
				return false;
			}

			FSaveMetadata Candidate;
			double Created = 0;
			double LastPlayed = 0;
			double GameTime = 0;
			double Schema = 0;
			double RulesVersion = 0;
			if (!Object->TryGetStringField(TEXT("slotId"), Candidate.SlotId) ||
				!Object->TryGetStringField(TEXT("playerLabel"), Candidate.PlayerLabel) ||
				!Object->TryGetStringField(TEXT("airportName"), Candidate.AirportName) ||
				!Object->TryGetStringField(TEXT("mapId"), Candidate.MapId) ||
				!Object->TryGetNumberField(TEXT("createdUnixSeconds"), Created) ||
				!Object->TryGetNumberField(TEXT("lastPlayedUnixSeconds"), LastPlayed) ||
				!Object->TryGetNumberField(TEXT("gameTimeMilliseconds"), GameTime) ||
				!Object->TryGetNumberField(TEXT("saveSchema"), Schema) ||
				!Object->TryGetNumberField(TEXT("simulationRulesVersion"), RulesVersion) ||
				!Object->TryGetStringField(TEXT("contentManifestHash"), Candidate.ContentManifestHash) ||
				!Object->TryGetStringField(TEXT("lastResult"), Candidate.LastResult))
			{
				return false;
			}
			Candidate.CreatedUnixSeconds = static_cast<int64>(Created);
			Candidate.LastPlayedUnixSeconds = static_cast<int64>(LastPlayed);
			Candidate.GameTimeMilliseconds = static_cast<int64>(GameTime);
			Candidate.SaveSchema = static_cast<uint32>(Schema);
			Candidate.SimulationRulesVersion = static_cast<uint32>(RulesVersion);

			FString Error;
			if (!FSaveStore::ValidateMetadata(Candidate, Error))
			{
				return false;
			}
			Output = MoveTemp(Candidate);
			return true;
		}

		FSaveResult WriteSnapshotAndMetadata(
			const FString& SlotDirectory,
			const FSnapshot& Snapshot,
			const FSaveMetadata& Metadata)
		{
			TArray<uint8> Bytes;
			FSnapshot Verification;
			if (!SerializeSnapshot(Snapshot, Bytes) || !DeserializeSnapshot(Bytes, Verification))
			{
				return {false, TEXT("Snapshot serialization verification failed.")};
			}

			FString MetadataJson;
			FString MetadataError;
			if (!FSaveStore::ValidateMetadata(Metadata, MetadataError) ||
				!SerializeMetadata(Metadata, MetadataJson))
			{
				return {false, MetadataError.IsEmpty() ? TEXT("Metadata serialization failed.") : MetadataError};
			}

			IFileManager& Files = IFileManager::Get();
			if (!Files.MakeDirectory(*SlotDirectory, true))
			{
				return {false, TEXT("Could not create slot directory.")};
			}

			const FString Current = FSaveStore::CurrentPath(SlotDirectory);
			const FString Backup = FSaveStore::BackupPath(SlotDirectory);
			const FString MetadataFile = FSaveStore::MetadataPath(SlotDirectory);
			const FString Temporary = Current + TEXT(".tmp");
			const FString TemporaryMetadata = MetadataFile + TEXT(".tmp");
			if (!FFileHelper::SaveArrayToFile(Bytes, *Temporary) ||
				!FFileHelper::SaveStringToFile(MetadataJson, *TemporaryMetadata))
			{
				Files.Delete(*Temporary, false, true);
				Files.Delete(*TemporaryMetadata, false, true);
				return {false, TEXT("Could not write temporary save set.")};
			}

			TArray<uint8> Written;
			FString WrittenMetadata;
			FSaveMetadata MetadataVerification;
			if (!FFileHelper::LoadFileToArray(Written, *Temporary) ||
				!DeserializeSnapshot(Written, Verification) ||
				!FFileHelper::LoadFileToString(WrittenMetadata, *TemporaryMetadata) ||
				!DeserializeMetadata(WrittenMetadata, MetadataVerification))
			{
				Files.Delete(*Temporary, false, true);
				Files.Delete(*TemporaryMetadata, false, true);
				return {false, TEXT("Temporary save-set verification failed.")};
			}

			if (Files.FileExists(*Current))
			{
				Files.Delete(*Backup, false, true);
				if (!Files.Move(*Backup, *Current, true, true))
				{
					Files.Delete(*Temporary, false, true);
					Files.Delete(*TemporaryMetadata, false, true);
					return {false, TEXT("Could not rotate current snapshot to backup.")};
				}
			}
			if (!Files.Move(*Current, *Temporary, true, true))
			{
				Files.Delete(*TemporaryMetadata, false, true);
				return {false, TEXT("Could not atomically replace current snapshot.")};
			}
			if (!Files.Move(*MetadataFile, *TemporaryMetadata, true, true))
			{
				return {false, TEXT("Could not atomically replace slot metadata.")};
			}
			return {true, {}};
		}
	}

	FString FSaveStore::CurrentPath(const FString& SlotDirectory)
	{
		return FPaths::Combine(SlotDirectory, TEXT("snapshot.ams"));
	}

	FString FSaveStore::BackupPath(const FString& SlotDirectory)
	{
		return FPaths::Combine(SlotDirectory, TEXT("snapshot.previous.ams"));
	}

	FString FSaveStore::MetadataPath(const FString& SlotDirectory)
	{
		return FPaths::Combine(SlotDirectory, TEXT("metadata.json"));
	}

	TFuture<FSaveResult> FSaveStore::WriteAsync(
		const FString& SlotDirectory,
		FSnapshot Snapshot,
		FSaveMetadata Metadata)
	{
		const FString NormalizedSlot = FPaths::ConvertRelativePathToFull(SlotDirectory);
		if (Metadata.SlotId.IsEmpty())
		{
			Metadata.SlotId = FPaths::GetCleanFilename(NormalizedSlot);
		}
		if (Metadata.CreatedUnixSeconds == 0)
		{
			Metadata.CreatedUnixSeconds = FDateTime::UtcNow().ToUnixTimestamp();
		}
		Metadata.LastPlayedUnixSeconds = FDateTime::UtcNow().ToUnixTimestamp();
		Metadata.GameTimeMilliseconds = Snapshot.GameTimeMilliseconds;
		Metadata.SaveSchema = Snapshot.SchemaVersion;

		TSharedPtr<FSlotWriteState, ESPMode::ThreadSafe> State;
		{
			FScopeLock Lock(&CoordinatorMutex);
			State = SlotStates.FindOrAdd(NormalizedSlot, MakeShared<FSlotWriteState, ESPMode::ThreadSafe>());
		}

		const TSharedPtr<TPromise<FSaveResult>, ESPMode::ThreadSafe> Promise =
			MakeShared<TPromise<FSaveResult>, ESPMode::ThreadSafe>();
		TFuture<FSaveResult> Future = Promise->GetFuture();
		bool bStartWorker = false;
		{
			FScopeLock Lock(&State->Mutex);
			State->PendingSnapshot = MoveTemp(Snapshot);
			State->PendingMetadata = MoveTemp(Metadata);
			State->PendingPromises.Add(Promise);
			if (!State->bWorkerRunning)
			{
				State->bWorkerRunning = true;
				bStartWorker = true;
			}
		}

		if (bStartWorker)
		{
			Async(EAsyncExecution::ThreadPool, [NormalizedSlot, State]()
			{
				for (;;)
				{
					FSnapshot NextSnapshot;
					FSaveMetadata NextMetadata;
					TArray<TSharedPtr<TPromise<FSaveResult>, ESPMode::ThreadSafe>> Promises;
					{
						FScopeLock Lock(&State->Mutex);
						NextSnapshot = MoveTemp(State->PendingSnapshot.GetValue());
						NextMetadata = MoveTemp(State->PendingMetadata.GetValue());
						State->PendingSnapshot.Reset();
						State->PendingMetadata.Reset();
						Promises = MoveTemp(State->PendingPromises);
					}

					const FSaveResult Result =
						WriteSnapshotAndMetadata(NormalizedSlot, NextSnapshot, NextMetadata);
					for (const TSharedPtr<TPromise<FSaveResult>, ESPMode::ThreadSafe>& PendingPromise : Promises)
					{
						PendingPromise->SetValue(Result);
					}

					FScopeLock Lock(&State->Mutex);
					if (!State->PendingSnapshot.IsSet())
					{
						State->bWorkerRunning = false;
						break;
					}
				}
			});
		}
		return Future;
	}

	bool FSaveStore::LoadCurrentOrBackup(const FString& SlotDirectory, FSnapshot& Output, bool& bUsedBackup)
	{
		for (const TPair<FString, bool>& Candidate : {
			TPair<FString, bool>{CurrentPath(SlotDirectory), false},
			TPair<FString, bool>{BackupPath(SlotDirectory), true}})
		{
			TArray<uint8> Bytes;
			if (FFileHelper::LoadFileToArray(Bytes, *Candidate.Key) && DeserializeSnapshot(Bytes, Output))
			{
				bUsedBackup = Candidate.Value;
				return true;
			}
		}
		return false;
	}

	bool FSaveStore::LoadMetadata(const FString& SlotDirectory, FSaveMetadata& Output)
	{
		FString Json;
		return FFileHelper::LoadFileToString(Json, *MetadataPath(SlotDirectory)) &&
			DeserializeMetadata(Json, Output);
	}

	bool FSaveStore::ValidateMetadata(const FSaveMetadata& Metadata, FString& Error)
	{
		if (Metadata.SlotId.IsEmpty() || Metadata.SlotId.Len() > 64 ||
			Metadata.SlotId.Contains(TEXT("/")) || Metadata.SlotId.Contains(TEXT("\\")) ||
			Metadata.SlotId.Contains(TEXT("..")))
		{
			Error = TEXT("Slot ID must be 1-64 safe local characters.");
			return false;
		}
		if (Metadata.PlayerLabel.Len() > 64 ||
			Metadata.AirportName.Len() > 128 ||
			Metadata.MapId.IsEmpty() || Metadata.MapId.Len() > 128 ||
			Metadata.ContentManifestHash.IsEmpty() || Metadata.ContentManifestHash.Len() > 128 ||
			Metadata.LastResult.Len() > 64)
		{
			Error = TEXT("Save metadata text exceeds its bounded field limit.");
			return false;
		}
		if (Metadata.CreatedUnixSeconds < 0 || Metadata.LastPlayedUnixSeconds < 0 ||
			Metadata.GameTimeMilliseconds < 0 ||
			Metadata.SaveSchema != SnapshotSchemaVersion ||
			Metadata.SimulationRulesVersion == 0)
		{
			Error = TEXT("Save metadata contains an unsupported version or time.");
			return false;
		}
		Error.Reset();
		return true;
	}
}
