#include "AMSimPhase1ConstructionPresentation.h"

#include "AMSimPhase1Fixture.h"

namespace AMSim
{
	FPhase1ConstructionVisualState FPhase1ConstructionPresentation::Derive(
		const FPhase1QuerySnapshot& Query,
		const FConstructionProjectRecord& Project)
	{
		FPhase1ConstructionVisualState Result;
		const EConstructionStage Stage = Project.Stage;
		if (Stage == EConstructionStage::None)
		{
			return Result;
		}

		const FPhase1Fixture& Fixture = GetPhase1Fixture();
		const int32 DurationPercent = HasConstructionRoadBenefit(Project.Proposal)
			? 100 - Fixture.ServiceRoadConstructionBonusPercent
			: 100;
		const auto Threshold = [DurationPercent](const int64 Value)
		{
			return Value * DurationPercent / 100;
		};
		const int64 Elapsed = FMath::Max<int64>(
			0,
			Query.GameTimeMilliseconds - Project.FundedAtGameMilliseconds);
		const int64 DeliveryAt = Threshold(Fixture.DeliveryAtMilliseconds);
		const int64 BuildingAt = Threshold(Fixture.BuildingAtMilliseconds);
		const int64 InspectionAt = Threshold(Fixture.InspectionAtMilliseconds);

		Result.DeliveryTravelProgress = DeliveryAt > 0
			? FMath::Clamp(static_cast<float>(Elapsed) / DeliveryAt, 0.0f, 1.0f)
			: 1.0f;
		if (Project.bDeliveryArrived || Stage >= EConstructionStage::Building)
		{
			Result.DeliveryTravelProgress = 1.0f;
		}

		if (Stage >= EConstructionStage::Inspection)
		{
			Result.SurfaceProgress = 1.0f;
		}
		else if (Stage == EConstructionStage::Building)
		{
			const int64 BuildDuration = FMath::Max<int64>(1, InspectionAt - BuildingAt);
			Result.SurfaceProgress = FMath::Clamp(
				static_cast<float>(Elapsed - BuildingAt) / BuildDuration,
				0.0f,
				1.0f);
		}

		const bool bActive =
			Stage == EConstructionStage::Funded ||
			Stage == EConstructionStage::AwaitingDelivery ||
			Stage == EConstructionStage::Building ||
			Stage == EConstructionStage::Inspection;
		Result.bCrewVisible = bActive;
		Result.bTruckVisible =
			Stage == EConstructionStage::Funded ||
			Stage == EConstructionStage::AwaitingDelivery ||
			Stage == EConstructionStage::Building;
		Result.bEarthworkVisible =
			Stage == EConstructionStage::Funded ||
			Stage == EConstructionStage::AwaitingDelivery ||
			Stage == EConstructionStage::Building;
		Result.bProtectionVisible = bActive;
		Result.bFinishedMarkingsVisible =
			Stage >= EConstructionStage::Inspection;
		return Result;
	}

	TArray<float> FPhase1ConstructionPresentation::AllocateLengthProgress(
		const float OverallProgress,
		const TConstArrayView<double> SegmentLengths)
	{
		TArray<float> Result;
		Result.Init(0.0f, SegmentLengths.Num());
		const float ClampedProgress = FMath::Clamp(OverallProgress, 0.0f, 1.0f);
		for (int32 Index = 0; Index < SegmentLengths.Num(); ++Index)
		{
			Result[Index] = SegmentLengths[Index] > UE_DOUBLE_SMALL_NUMBER
				? ClampedProgress
				: 0.0f;
		}
		return Result;
	}
}
