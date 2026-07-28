#include "AMSimWorldPresenter.h"

#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

namespace
{
	constexpr float TerminalHorizontalSpread = 1.30f;
	constexpr float SpritePlaneRoll = -90.0f;

	FVector SpreadTerminalPosition(FVector Position)
	{
		Position.Y *= TerminalHorizontalSpread;
		return Position;
	}
}

void AAMSimWorldPresenter::FinalizeTerminalPresentation(
	UPaperSprite* DirectionArrowSprite,
	UPaperSprite* AccessibleRouteSprite,
	UPaperSprite* SecureDoorSprite,
	UPaperSprite* PartitionWallSprite,
	UPaperSprite* CautionHatchSprite,
	UPaperSprite* SortingTableSprite)
{
	const auto SpreadStructure =
		[](UPaperSpriteComponent* Component, const bool bWidenSurface)
		{
			Component->SetRelativeLocation(
				SpreadTerminalPosition(Component->GetRelativeLocation()));
			if (bWidenSurface)
			{
				FVector Scale = Component->GetRelativeScale3D();
				Scale.Z *= 1.12;
				Component->SetRelativeScale3D(Scale);
			}
		};
	for (UPaperSpriteComponent* Component : Phase3Floor)
	{
		SpreadStructure(Component, true);
	}
	for (UPaperSpriteComponent* Component : Phase3Rooms)
	{
		SpreadStructure(Component, true);
	}
	for (UPaperSpriteComponent* Component : Phase3SecurityBoundary)
	{
		SpreadStructure(Component, true);
	}
	for (UPaperSpriteComponent* Component : Phase3Congestion)
	{
		SpreadStructure(Component, true);
	}
	for (UPaperSpriteComponent* Component : Phase3Props)
	{
		SpreadStructure(Component, false);
	}
	for (UPaperSpriteComponent* Component : Phase3Vehicles)
	{
		SpreadStructure(Component, false);
	}
	for (UPaperSpriteComponent* Component : Phase3Staff)
	{
		SpreadStructure(Component, false);
	}

	const FVector DeparturePoints[] = {
		FVector(-27000.0, -25000.0, 52.0),
		FVector(-12000.0, -24000.0, 52.0),
		FVector(0.0, -15000.0, 52.0),
		FVector(5000.0, -3000.0, 52.0),
		FVector(17000.0, -8000.0, 52.0),
		FVector(27000.0, -20000.0, 52.0)};
	const FVector AccessiblePoints[] = {
		FVector(-25800.0, -25000.0, 56.0),
		FVector(-10800.0, -24000.0, 56.0),
		FVector(1200.0, -15000.0, 56.0),
		FVector(6200.0, -3000.0, 56.0),
		FVector(18200.0, -8000.0, 56.0),
		FVector(28200.0, -20000.0, 56.0)};
	const FVector ArrivalPoints[] = {
		FVector(27000.0, 7000.0, 53.0),
		FVector(6000.0, 11000.0, 53.0),
		FVector(-11000.0, 7000.0, 53.0),
		FVector(-17000.0, -9000.0, 53.0),
		FVector(-27000.0, -25000.0, 53.0)};
	const FVector BaggagePoints[] = {
		FVector(-2000.0, -21000.0, 51.0),
		FVector(3000.0, -9000.0, 51.0),
		FVector(5000.0, 8000.0, 51.0),
		FVector(12000.0, 16000.0, 51.0),
		FVector(-4000.0, 10000.0, 51.0),
		FVector(-16000.0, -10000.0, 51.0)};
	const FVector LandsidePoints[] = {
		FVector(-35000.0, -33000.0, 50.0),
		FVector(-35000.0, -15000.0, 50.0),
		FVector(-35000.0, 3000.0, 50.0),
		FVector(-35000.0, 21000.0, 50.0)};
	const FVector SecurityBoundaryLocations[] = {
		FVector(-9000.0, -10000.0, 49.0),
		FVector(-9000.0, 4500.0, 49.0),
		FVector(9000.0, -10000.0, 49.0),
		FVector(9000.0, 4500.0, 49.0),
		FVector(0.0, -11000.0, 50.0),
		FVector(10000.0, -3000.0, 50.0),
		FVector(0.0, 6000.0, 50.0),
		FVector(-10000.0, -3000.0, 50.0)};
	const FVector BaggageExceptionPoints[] = {
		FVector(5000.0, 8000.0, 55.0),
		FVector(10500.0, 10500.0, 55.0),
		FVector(15500.0, 13500.0, 55.0),
		FVector(20500.0, 16500.0, 55.0)};

	const auto ConfigureConnectedRoute =
		[this, DirectionArrowSprite](
			const TArray<TObjectPtr<UPaperSpriteComponent>>& Components,
			const FVector* Points,
			const int32 PointCount,
			const float Thickness,
			UPaperSprite* SegmentSprite)
		{
			const int32 SegmentCount = PointCount - 1;
			for (int32 Index = 0; Index < Components.Num(); ++Index)
			{
				UPaperSpriteComponent* Component = Components[Index];
				const int32 SegmentIndex =
					FMath::Min(Index, SegmentCount - 1);
				const FVector Start =
					SpreadTerminalPosition(Points[SegmentIndex]);
				const FVector End =
					SpreadTerminalPosition(Points[SegmentIndex + 1]);
				const FVector Delta = End - Start;
				const float Length =
					FVector2D(Delta.X, Delta.Y).Size();
				if (Index < SegmentCount)
				{
					ConfigureSprite(
						Component,
						SegmentSprite,
						(Start + End) * 0.5,
						FVector(Length / 200.0f, 1.0f, Thickness));
				}
				else
				{
					ConfigureSprite(
						Component,
						DirectionArrowSprite,
						End,
						FVector(6.0f, 1.0f, 6.0f));
				}
				const float Angle = FMath::RadiansToDegrees(
					FMath::Atan2(Delta.Y, Delta.X));
				Component->SetRelativeRotation(
					FRotator(0.0f, Angle, SpritePlaneRoll));
			}
		};
	ConfigureConnectedRoute(
		Phase3DepartureFlow,
		DeparturePoints,
		UE_ARRAY_COUNT(DeparturePoints),
		3.4f,
		WhiteSprite);
	ConfigureConnectedRoute(
		Phase3ArrivalFlow,
		ArrivalPoints,
		UE_ARRAY_COUNT(ArrivalPoints),
		3.4f,
		WhiteSprite);
	ConfigureConnectedRoute(
		Phase3BaggageFlow,
		BaggagePoints,
		UE_ARRAY_COUNT(BaggagePoints),
		3.0f,
		WhiteSprite);
	ConfigureConnectedRoute(
		Phase3BaggageExceptionRoute,
		BaggageExceptionPoints,
		UE_ARRAY_COUNT(BaggageExceptionPoints),
		2.7f,
		WhiteSprite);
	ConfigureConnectedRoute(
		Phase3AccessibleFlow,
		AccessiblePoints,
		UE_ARRAY_COUNT(AccessiblePoints),
		0.7f,
		AccessibleRouteSprite);
	for (int32 Index = 0; Index < Phase3AccessibleDashes.Num(); ++Index)
	{
		const int32 SegmentIndex = Index / 4;
		const int32 DashIndex = Index % 4;
		const FVector Start =
			SpreadTerminalPosition(AccessiblePoints[SegmentIndex]);
		const FVector End =
			SpreadTerminalPosition(AccessiblePoints[SegmentIndex + 1]);
		const FVector Delta = End - Start;
		const float SegmentLength =
			FVector2D(Delta.X, Delta.Y).Size();
		const float Alpha = 0.14f + DashIndex * 0.24f;
		ConfigureSprite(
			Phase3AccessibleDashes[Index],
			WhiteSprite,
			FMath::Lerp(Start, End, Alpha),
			FVector(SegmentLength / 2000.0f, 1.0f, 2.7f));
		const float Angle = FMath::RadiansToDegrees(
			FMath::Atan2(Delta.Y, Delta.X));
		Phase3AccessibleDashes[Index]->SetRelativeRotation(
			FRotator(0.0f, Angle, SpritePlaneRoll));
	}
	ConfigureConnectedRoute(
		Phase3LandsideFlow,
		LandsidePoints,
		UE_ARRAY_COUNT(LandsidePoints),
		3.4f,
		WhiteSprite);
	for (int32 Index = 0; Index < Phase3SecurityBoundary.Num(); ++Index)
	{
		const bool bDoor = Index >= 4;
		ConfigureSprite(
			Phase3SecurityBoundary[Index],
			bDoor ? SecureDoorSprite : PartitionWallSprite,
			SpreadTerminalPosition(SecurityBoundaryLocations[Index]),
			bDoor
				? FVector(8.5f, 1.0f, 8.5f)
				: FVector(10.0f, 1.0f, 10.0f));
		Phase3SecurityBoundary[Index]->SetSpriteColor(FLinearColor::White);
	}
	ConfigureSprite(
		Phase3BaggageExceptionZone,
		CautionHatchSprite,
		SpreadTerminalPosition(FVector(20500.0, 16500.0, 54.0)),
		FVector(12.0f, 1.0f, 12.0f));
	ConfigureSprite(
		Phase3BaggageExceptionStation,
		SortingTableSprite,
		SpreadTerminalPosition(FVector(19500.0, 15500.0, 58.0)),
		FVector(7.5f, 1.0f, 7.5f));
	Phase3BaggageExceptionZone->SetSpriteColor(
		FLinearColor(0.72f, 0.46f, 0.24f, 0.52f));
	Phase3BaggageExceptionStation->SetSpriteColor(FLinearColor::White);
}
