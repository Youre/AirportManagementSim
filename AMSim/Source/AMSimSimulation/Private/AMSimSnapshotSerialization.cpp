#include "AMSimSnapshotSerialization.h"
#include "Misc/Crc.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

namespace AMSim
{
	namespace
	{
		constexpr uint32 SnapshotMagic = 0x414D5331;

		void SerializeBody(FArchive& Archive, FSnapshot& Snapshot)
		{
			Archive << Snapshot.SchemaVersion;
			Archive << Snapshot.MasterSeed;
			Archive << Snapshot.NextEntityId;
			Archive << Snapshot.NextEventSequence;
			Archive << Snapshot.Revision;
			Archive << Snapshot.GameTimeMilliseconds;
			int32 EntityCount = Snapshot.Entities.Num();
			Archive << EntityCount;
			if (Archive.IsLoading())
			{
				if (EntityCount < 0 || EntityCount > 1000000)
				{
					Archive.SetError();
					return;
				}
				Snapshot.Entities.SetNum(EntityCount);
			}
			for (FEntityId& Entity : Snapshot.Entities)
			{
				Archive << Entity.Value;
			}
		}
	}

	bool SerializeSnapshot(const FSnapshot& Snapshot, TArray<uint8>& Output)
	{
		TArray<uint8> Body;
		FMemoryWriter BodyWriter(Body, true);
		FSnapshot MutableSnapshot = Snapshot;
		SerializeBody(BodyWriter, MutableSnapshot);
		if (BodyWriter.IsError())
		{
			return false;
		}

		Output.Reset();
		FMemoryWriter Writer(Output, true);
		uint32 Magic = SnapshotMagic;
		uint32 BodySize = Body.Num();
		uint32 Checksum = FCrc::MemCrc32(Body.GetData(), Body.Num());
		Writer << Magic;
		Writer << BodySize;
		Writer << Checksum;
		Writer.Serialize(Body.GetData(), Body.Num());
		return !Writer.IsError();
	}

	bool DeserializeSnapshot(const TArray<uint8>& Input, FSnapshot& Output)
	{
		FMemoryReader Reader(Input, true);
		uint32 Magic = 0;
		uint32 BodySize = 0;
		uint32 ExpectedChecksum = 0;
		Reader << Magic;
		Reader << BodySize;
		Reader << ExpectedChecksum;
		if (Reader.IsError() ||
			Magic != SnapshotMagic ||
			BodySize != static_cast<uint32>(Reader.TotalSize() - Reader.Tell()))
		{
			return false;
		}

		TArray<uint8> Body;
		Body.SetNumUninitialized(BodySize);
		Reader.Serialize(Body.GetData(), BodySize);
		if (Reader.IsError() || FCrc::MemCrc32(Body.GetData(), Body.Num()) != ExpectedChecksum)
		{
			return false;
		}

		FMemoryReader BodyReader(Body, true);
		FSnapshot Candidate;
		SerializeBody(BodyReader, Candidate);
		if (BodyReader.IsError() || Candidate.SchemaVersion != SnapshotSchemaVersion)
		{
			return false;
		}
		Output = MoveTemp(Candidate);
		return true;
	}
}
