#include "AMSimWorldPresenter.h"

#include "Algo/Count.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	constexpr float SpritePlaneRoll = -90.0f;

	void SetPoolVisible(
		const TArray<TObjectPtr<UPaperSpriteComponent>>& Pool,
		const int32 VisibleCount)
	{
		for (int32 Index = 0; Index < Pool.Num(); ++Index)
		{
			Pool[Index]->SetVisibility(Index < VisibleCount);
		}
	}

	UPaperSprite* FindPhase6Sprite(const TCHAR* ObjectPath)
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperSprite> Finder(
			ObjectPath);
		return Finder.Succeeded() ? Finder.Get() : nullptr;
	}

	int32 GetLargeAircraftHeadingIndex(
		const AMSim::ELargeAircraftOperationState State)
	{
		switch (State)
		{
		case AMSim::ELargeAircraftOperationState::Scheduled:
			return 8;
		case AMSim::ELargeAircraftOperationState::DepartureReady:
			return 12;
		case AMSim::ELargeAircraftOperationState::Departed:
			return 0;
		default:
			return 4;
		}
	}
}

void AAMSimWorldPresenter::InitializePhase6Presentation()
{
	for (int32 Index = 0; Index < 16; ++Index)
	{
		const FString Path = FString::Printf(
			TEXT("/Game/Phase6/Presentation/Sprites/Aircraft/"
				"S_RiverbendLongreach787_9_H%02d."
				"S_RiverbendLongreach787_9_H%02d"),
			Index,
			Index);
		Phase6WidebodySprites.Add(FindPhase6Sprite(*Path));
	}

	const FVector RunwayLocations[] = {
		FVector(-8500.0f, -9000.0f, 31.0f),
		FVector(-8500.0f, 9000.0f, 32.0f)};
	for (int32 Index = 0; Index < 2; ++Index)
	{
		UPaperSpriteComponent* Component = CreateSpriteComponent(
			*FString::Printf(TEXT("Phase6Runway%d"), Index),
			31 + Index,
			FLinearColor(0.18f, 0.22f, 0.27f, 1.0f));
		ConfigureSprite(
			Component,
			WhiteSprite,
			RunwayLocations[Index],
			FVector(58.0f, 1.0f, 3.5f));
		Component->SetRelativeRotation(
			FRotator(0.0f, 90.0f, SpritePlaneRoll));
		Component->SetVisibility(false);
		Phase6RunwaySystem.Add(Component);
	}

	for (int32 Index = 0; Index < 12; ++Index)
	{
		const int32 Column = Index % 4;
		const int32 Row = Index / 4;
		UPaperSpriteComponent* Component = CreateSpriteComponent(
			*FString::Printf(TEXT("Phase6Facility%d"), Index),
			46 + Row,
			FLinearColor(0.10f, 0.73f, 0.80f, 0.95f));
		ConfigureSprite(
			Component,
			WhiteSprite,
			FVector(
				12500.0f + Column * 8500.0f,
				-25500.0f + Row * 15500.0f,
				46.0f + Row),
			FVector(
				Index == 6 || Index == 7 ? 9.0f : 6.5f,
				1.0f,
				Index == 6 || Index == 7 ? 8.0f : 5.0f));
		Component->SetVisibility(false);
		Phase6Facilities.Add(Component);
	}

	UPaperSpriteComponent* WideBody = CreateSpriteComponent(
		TEXT("Phase6WideBodyCandidate"),
		72,
		FLinearColor(0.90f, 0.95f, 0.98f, 1.0f));
	ConfigureSprite(
		WideBody,
		Phase6WidebodySprites.IsValidIndex(4)
			? Phase6WidebodySprites[4]
			: WhiteSprite,
		FVector(21500.0f, 6500.0f, 72.0f),
		FVector(1.0f));
	WideBody->SetRelativeRotation(
		FRotator(0.0f, 0.0f, SpritePlaneRoll));
	WideBody->SetVisibility(false);
	Phase6LargeAircraft.Add(WideBody);

	const FLinearColor ResponseColors[] = {
		FLinearColor(0.96f, 0.22f, 0.16f, 1.0f),
		FLinearColor(0.96f, 0.74f, 0.18f, 1.0f),
		FLinearColor(0.16f, 0.70f, 0.82f, 1.0f),
		FLinearColor(0.90f, 0.95f, 0.98f, 1.0f)};
	for (int32 Index = 0; Index < 4; ++Index)
	{
		UPaperSpriteComponent* Component = CreateSpriteComponent(
			*FString::Printf(TEXT("Phase6Response%d"), Index),
			84 + Index,
			ResponseColors[Index]);
		ConfigureSprite(
			Component,
			WhiteSprite,
			FVector(
				-25000.0f + Index * 4300.0f,
				-9000.0f,
				84.0f + Index),
			FVector(3.2f, 1.0f, 1.7f));
		Component->SetVisibility(false);
		Phase6Response.Add(Component);
	}
	for (int32 Index = 0; Index < 6; ++Index)
	{
		UPaperSpriteComponent* Component = CreateSpriteComponent(
			*FString::Printf(TEXT("Phase6Closure%d"), Index),
			82,
			FLinearColor(0.96f, 0.22f, 0.16f, 0.78f));
		ConfigureSprite(
			Component,
			WhiteSprite,
			FVector(
				-28000.0f + Index * 7000.0f,
				-9000.0f,
				82.0f),
			FVector(3.8f, 1.0f, 0.55f));
		Component->SetRelativeRotation(
			FRotator(0.0f, Index % 2 == 0 ? 28.0f : -28.0f, SpritePlaneRoll));
		Component->SetVisibility(false);
		Phase6Closure.Add(Component);
	}
}

void AAMSimWorldPresenter::ApplyPhase6Snapshot(
	const AMSim::FPhase6QuerySnapshot& Query,
	const AMSim::FPhase6State& State)
{
	if (LastAppliedPhase6Revision == Query.Revision)
	{
		return;
	}
	LastAppliedPhase6Revision = Query.Revision;
	if (!Query.bInitialized)
	{
		SetPoolVisible(Phase6RunwaySystem, 0);
		SetPoolVisible(Phase6Facilities, 0);
		SetPoolVisible(Phase6LargeAircraft, 0);
		SetPoolVisible(Phase6Response, 0);
		SetPoolVisible(Phase6Closure, 0);
		return;
	}

	SetPoolVisible(Phase6RunwaySystem, State.Runways.Num());
	for (int32 Index = 0; Index < Phase6RunwaySystem.Num(); ++Index)
	{
		const bool bOperational =
			State.Runways.IsValidIndex(Index) &&
			State.Runways[Index].bOperational;
		Phase6RunwaySystem[Index]->SetSpriteColor(
			bOperational
				? FLinearColor(0.20f, 0.25f, 0.31f, 1.0f)
				: FLinearColor(0.45f, 0.37f, 0.20f, 0.52f));
	}
	SetPoolVisible(
		Phase6Facilities,
		FMath::Min(State.Facilities.Num(), Phase6Facilities.Num()));
	for (int32 Index = 0; Index < Phase6Facilities.Num(); ++Index)
	{
		if (!State.Facilities.IsValidIndex(Index))
		{
			continue;
		}
		const AMSim::EMajorFacilityState FacilityState =
			State.Facilities[Index].State;
		Phase6Facilities[Index]->SetSpriteColor(
			FacilityState == AMSim::EMajorFacilityState::Operational
				? FLinearColor(0.12f, 0.72f, 0.58f, 0.98f)
				: FacilityState ==
						AMSim::EMajorFacilityState::UnderConstruction
					? FLinearColor(0.96f, 0.65f, 0.16f, 0.88f)
					: FLinearColor(0.10f, 0.68f, 0.78f, 0.34f));
	}
	const bool bShowLargeAircraft =
		State.LargeAircraftOperations.ContainsByPredicate(
			[](const AMSim::FLargeAircraftOperationRecord& Operation)
			{
				return Operation.State !=
						AMSim::ELargeAircraftOperationState::Offered &&
					Operation.State !=
						AMSim::ELargeAircraftOperationState::Completed;
			});
	SetPoolVisible(Phase6LargeAircraft, bShowLargeAircraft ? 1 : 0);
	if (bShowLargeAircraft && !Phase6LargeAircraft.IsEmpty())
	{
		const AMSim::FLargeAircraftOperationRecord* Operation =
			State.LargeAircraftOperations.FindByPredicate(
				[](const AMSim::FLargeAircraftOperationRecord& Candidate)
				{
					return Candidate.State !=
							AMSim::ELargeAircraftOperationState::Offered &&
						Candidate.State !=
							AMSim::ELargeAircraftOperationState::Completed;
				});
		const int32 HeadingIndex = Operation
			? GetLargeAircraftHeadingIndex(Operation->State)
			: 4;
		if (Phase6WidebodySprites.IsValidIndex(HeadingIndex))
		{
			Phase6LargeAircraft[0]->SetSprite(
				Phase6WidebodySprites[HeadingIndex]);
			Phase6LargeAircraft[0]->SetSpriteColor(FLinearColor::White);
		}
	}

	const bool bIncidentActive =
		State.SeriousIncident.Lifecycle >=
			AMSim::ESeriousIncidentLifecycle::Materialized &&
		State.SeriousIncident.Lifecycle !=
			AMSim::ESeriousIncidentLifecycle::Recovered;
	SetPoolVisible(
		Phase6Response,
		bIncidentActive
			? State.SeriousIncident.bAreaProtected
				? Phase6Response.Num()
				: 2
			: 0);
	SetPoolVisible(
		Phase6Closure,
		bIncidentActive ? Phase6Closure.Num() : 0);
}

int32 AAMSimWorldPresenter::GetActivePhase6ProxyCount() const
{
	const auto CountVisible =
		[](const TArray<TObjectPtr<UPaperSpriteComponent>>& Pool)
		{
			return Algo::CountIf(
				Pool,
				[](const UPaperSpriteComponent* Component)
				{
					return Component && Component->IsVisible();
				});
		};
	return CountVisible(Phase6RunwaySystem) +
		CountVisible(Phase6Facilities) +
		CountVisible(Phase6LargeAircraft) +
		CountVisible(Phase6Response) +
		CountVisible(Phase6Closure);
}
