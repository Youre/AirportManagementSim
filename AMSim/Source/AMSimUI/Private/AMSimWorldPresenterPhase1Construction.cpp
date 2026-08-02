#include "AMSimWorldPresenter.h"

#include "AMSimPhase1ConstructionPresentation.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimPhase1WorldGeometry.h"
#include "Components/TextRenderComponent.h"
#include "PaperSpriteComponent.h"

namespace
{
	constexpr float SpritePlaneRoll = -90.0f;

	double SegmentLength(
		const AMSim::FPhase1Point& Start,
		const AMSim::FPhase1Point& End)
	{
		return FVector2D(
			static_cast<double>(End.X - Start.X),
			static_cast<double>(End.Y - Start.Y)).Size();
	}

	AMSim::FPhase1Point PointAt(
		const AMSim::FPhase1Point& Start,
		const AMSim::FPhase1Point& End,
		const float Progress)
	{
		return {
			FMath::RoundToInt64(FMath::Lerp(
				static_cast<double>(Start.X),
				static_cast<double>(End.X),
				Progress)),
			FMath::RoundToInt64(FMath::Lerp(
				static_cast<double>(Start.Y),
				static_cast<double>(End.Y),
				Progress))};
	}

	void ApplyPartialSegment(
		UPaperSpriteComponent* Component,
		const AMSim::FPhase1Point& Start,
		const AMSim::FPhase1Point& End,
		const int64 WidthCentimeters,
		const double Height,
		const float Progress,
		const bool bAllowed)
	{
		if (!Component)
		{
			return;
		}
		const bool bVisible = bAllowed && Progress > KINDA_SMALL_NUMBER;
		Component->SetVisibility(bVisible);
		if (!bVisible)
		{
			return;
		}

		const AMSim::FPhase1WorldSegmentGeometry Geometry =
			AMSim::MakePhase1WorldSegmentGeometry(
				Start,
				PointAt(Start, End, Progress),
				WidthCentimeters,
				Height);
		Component->SetRelativeLocation(Geometry.Center);
		Component->SetRelativeScale3D(Geometry.Scale);
		Component->SetRelativeRotation(
			FRotator(0.0f, Geometry.YawDegrees, SpritePlaneRoll));
	}

	FVector WithHeight(FVector Location, const double Height)
	{
		Location.Z = Height;
		return Location;
	}
}

void AAMSimWorldPresenter::InitializePhase1ConstructionPresentation()
{
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Phase1ConstructionProxies.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionProxy%d"), Index),
			54 + Index));
	}
	Phase1RunwayEarthwork = CreateSpriteComponent(
		TEXT("Phase1RunwayEarthwork"),
		9,
		FLinearColor(0.64f, 0.45f, 0.25f, 0.92f));
	for (int32 Index = 0; Index < 8; ++Index)
	{
		Phase1TaxiwayEarthworks.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1TaxiwayEarthwork%d"), Index),
			19 + Index,
			FLinearColor(0.69f, 0.49f, 0.27f, 0.94f)));
	}
	Phase1RoadEarthwork = CreateSpriteComponent(
		TEXT("Phase1RoadEarthwork"),
		19,
		FLinearColor(0.58f, 0.40f, 0.24f, 0.92f));
	for (int32 Index = 0; Index < 3; ++Index)
	{
		Phase1ConstructionCrew.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionCrew%d"), Index),
			58 + Index));
	}
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Phase1ConstructionBoundaryMarkers.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase1ConstructionBoundary%d"), Index),
			53 + Index));
	}
}

void AAMSimWorldPresenter::RefreshPhase1ConstructionPresentation(
	const AMSim::FPhase1QuerySnapshot& Query,
	const AMSim::FPhase1State& State)
{
	const AMSim::FPhase1ConstructionVisualState Visual =
		AMSim::FPhase1ConstructionPresentation::Derive(Query, State.Project);
	Phase1ConstructionTravelProgress = Visual.DeliveryTravelProgress;
	Phase1ConstructionSurfaceProgress = Visual.SurfaceProgress;

	const bool bWorldAllowed =
		bRequestedPhase1NetworkVisible && !bConstructionEditorOverlayVisible;
	const AMSim::FStarterPlanProposal& Proposal = State.Project.Proposal;
	const TArray<AMSim::FTaxiwaySegment> TaxiSegments =
		AMSim::GetTaxiwaySegments(Proposal);

	TArray<double> SegmentLengths;
	SegmentLengths.Reserve(2 + TaxiSegments.Num());
	SegmentLengths.Add(SegmentLength(Proposal.RunwayStart, Proposal.RunwayEnd));
	for (const AMSim::FTaxiwaySegment& Segment : TaxiSegments)
	{
		SegmentLengths.Add(SegmentLength(Segment.Start, Segment.End));
	}
	const bool bRoadPresent = Proposal.AccessStart != Proposal.AccessEnd;
	if (bRoadPresent)
	{
		SegmentLengths.Add(SegmentLength(Proposal.AccessStart, Proposal.AccessEnd));
	}
	const TArray<float> SegmentProgress =
		AMSim::FPhase1ConstructionPresentation::AllocateLengthProgress(
			Visual.SurfaceProgress,
			SegmentLengths);

	int32 ProgressIndex = 0;
	ApplyPartialSegment(
		Runway,
		Proposal.RunwayStart,
		Proposal.RunwayEnd,
		Proposal.RunwayWidthCentimeters,
		10.0,
		SegmentProgress.IsValidIndex(ProgressIndex)
			? SegmentProgress[ProgressIndex]
			: 0.0f,
		bWorldAllowed);
	++ProgressIndex;
	Runway->SetSpriteColor(FLinearColor::White);

	for (int32 Index = 0; Index < Phase1TaxiwaySegments.Num(); ++Index)
	{
		const bool bHasSegment = TaxiSegments.IsValidIndex(Index);
		const float Progress = bHasSegment && SegmentProgress.IsValidIndex(ProgressIndex)
			? SegmentProgress[ProgressIndex]
			: 0.0f;
		if (bHasSegment)
		{
			ApplyPartialSegment(
				Phase1TaxiwaySegments[Index],
				TaxiSegments[Index].Start,
				TaxiSegments[Index].End,
				1200,
				20.0 + Index,
				Progress,
				bWorldAllowed);
			++ProgressIndex;
		}
		else
		{
			Phase1TaxiwaySegments[Index]->SetVisibility(false);
		}
		Phase1TaxiwaySegments[Index]->SetSpriteColor(FLinearColor::White);
	}

	const float RoadProgress =
		bRoadPresent && SegmentProgress.IsValidIndex(ProgressIndex)
			? SegmentProgress[ProgressIndex]
			: 0.0f;
	ApplyPartialSegment(
		Access,
		Proposal.AccessStart,
		Proposal.AccessEnd,
		1000,
		20.0,
		RoadProgress,
		bWorldAllowed && bRoadPresent);
	Access->SetSpriteColor(FLinearColor::White);

	const bool bShowEarthwork =
		bWorldAllowed && Visual.bEarthworkVisible;
	Phase1RunwayEarthwork->SetVisibility(bShowEarthwork);
	for (int32 Index = 0; Index < Phase1TaxiwayEarthworks.Num(); ++Index)
	{
		Phase1TaxiwayEarthworks[Index]->SetVisibility(
			bShowEarthwork && TaxiSegments.IsValidIndex(Index));
	}
	Phase1RoadEarthwork->SetVisibility(bShowEarthwork && bRoadPresent);

	const bool bShowMarkings =
		bWorldAllowed && Visual.bFinishedMarkingsVisible;
	RunwayStartNumber->SetVisibility(bShowMarkings);
	RunwayEndNumber->SetVisibility(bShowMarkings);
	Windsock->SetVisibility(bShowMarkings);

	const FVector TerminalOrigin = AMSim::MapPhase1PointToWorld(
		AMSim::GetStarterTerminalCenter(),
		56.0);
	const FVector RunwayStart = AMSim::MapPhase1PointToWorld(
		Proposal.RunwayStart,
		56.0);
	const FVector RunwayEnd = AMSim::MapPhase1PointToWorld(
		Proposal.RunwayEnd,
		56.0);
	const FVector TaxiTarget = !TaxiSegments.IsEmpty()
		? AMSim::MapPhase1PointToWorld(
			PointAt(TaxiSegments[0].Start, TaxiSegments[0].End, 0.55f),
			57.0)
		: Phase1StandCenter;
	const FVector CrewTargets[] = {
		FMath::Lerp(RunwayStart, RunwayEnd, 0.22f),
		FMath::Lerp(RunwayStart, RunwayEnd, 0.72f),
		TaxiTarget,
		Phase1StandCenter};
	const float TravelAlpha = FMath::InterpEaseInOut(
		0.0f,
		1.0f,
		Visual.DeliveryTravelProgress,
		2.0f);
	TArray<UPaperSpriteComponent*> Workers;
	Workers.Add(Phase1ConstructionProxies.IsValidIndex(1)
		? Phase1ConstructionProxies[1].Get()
		: nullptr);
	for (UPaperSpriteComponent* Crew : Phase1ConstructionCrew)
	{
		Workers.Add(Crew);
	}
	for (int32 Index = 0; Index < Workers.Num(); ++Index)
	{
		UPaperSpriteComponent* Worker = Workers[Index];
		if (!Worker)
		{
			continue;
		}
		const FVector DepartureOffset(
			(Index - 1.5f) * 750.0f,
			(Index % 2 == 0 ? -650.0f : 650.0f),
			0.0f);
		FVector WorkerLocation = FMath::Lerp(
			TerminalOrigin + DepartureOffset,
			CrewTargets[Index],
			TravelAlpha);
		if (State.Project.Stage == AMSim::EConstructionStage::Building)
		{
			const float WorkAlpha = FMath::Frac(
				Visual.SurfaceProgress + Index * 0.21f);
			WorkerLocation = Index < 2
				? FMath::Lerp(RunwayStart, RunwayEnd, WorkAlpha)
				: FMath::Lerp(TaxiTarget, Phase1StandCenter, WorkAlpha);
		}
		Worker->SetRelativeLocation(WithHeight(WorkerLocation, 56.0 + Index));
		Worker->SetVisibility(bWorldAllowed && Visual.bCrewVisible);
	}

	if (Phase1ConstructionProxies.IsValidIndex(0))
	{
		UPaperSpriteComponent* Truck = Phase1ConstructionProxies[0];
		const AMSim::FPhase1Point TerminalPoint =
			AMSim::GetStarterTerminalCenter();
		const bool bAccessStartNearTerminal =
			SegmentLength(TerminalPoint, Proposal.AccessStart) <=
			SegmentLength(TerminalPoint, Proposal.AccessEnd);
		const FVector TruckOrigin = bRoadPresent
			? AMSim::MapPhase1PointToWorld(
				bAccessStartNearTerminal
					? Proposal.AccessEnd
					: Proposal.AccessStart,
				57.0)
			: TerminalOrigin + FVector(3500.0, 2500.0, 1.0);
		const FVector TruckTarget = bRoadPresent
			? AMSim::MapPhase1PointToWorld(
				bAccessStartNearTerminal
					? Proposal.AccessStart
					: Proposal.AccessEnd,
				57.0)
			: RunwayStart + FVector(4500.0, 2500.0, 1.0);
		Truck->SetRelativeLocation(FMath::Lerp(TruckOrigin, TruckTarget, TravelAlpha));
		Truck->SetVisibility(bWorldAllowed && Visual.bTruckVisible);
	}
	if (Phase1ConstructionProxies.IsValidIndex(2))
	{
		Phase1ConstructionProxies[2]->SetVisibility(
			bWorldAllowed && Visual.bProtectionVisible);
	}
	if (Phase1ConstructionProxies.IsValidIndex(3))
	{
		Phase1ConstructionProxies[3]->SetVisibility(
			bWorldAllowed && Visual.bProtectionVisible);
	}
	for (UPaperSpriteComponent* Marker : Phase1ConstructionBoundaryMarkers)
	{
		Marker->SetVisibility(bWorldAllowed && Visual.bProtectionVisible);
	}
}

int32 AAMSimWorldPresenter::GetActivePhase1EarthworkProxyCount() const
{
	int32 Count = Phase1RunwayEarthwork && Phase1RunwayEarthwork->IsVisible()
		? 1
		: 0;
	for (const UPaperSpriteComponent* Component : Phase1TaxiwayEarthworks)
	{
		Count += Component && Component->IsVisible() ? 1 : 0;
	}
	Count += Phase1RoadEarthwork && Phase1RoadEarthwork->IsVisible() ? 1 : 0;
	return Count;
}
