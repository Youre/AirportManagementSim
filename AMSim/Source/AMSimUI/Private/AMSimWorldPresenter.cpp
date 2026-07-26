#include "AMSimWorldPresenter.h"

#include "Algo/AllOf.h"
#include "AMSimAircraftPresentation.h"
#include "Components/SceneComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	UPaperSprite* FindSprite(const TCHAR* ObjectPath)
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperSprite> Finder(ObjectPath);
		return Finder.Get();
	}

	constexpr float SpritePlaneRoll = -90.0f;
}

AAMSimWorldPresenter::AAMSimWorldPresenter()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Terrain = CreateSpriteComponent(TEXT("Terrain"), 0);
	Runway = CreateSpriteComponent(TEXT("Runway"), 10);
	Taxiway = CreateSpriteComponent(TEXT("Taxiway"), 20);
	Stand = CreateSpriteComponent(TEXT("Stand"), 30);
	Access = CreateSpriteComponent(TEXT("Access"), 20);
	OperationsHut = CreateSpriteComponent(TEXT("OperationsHut"), 40);
	Windsock = CreateSpriteComponent(TEXT("Windsock"), 45);
	Aircraft = CreateSpriteComponent(TEXT("Aircraft"), 60);
	Selection = CreateSpriteComponent(TEXT("Selection"), 70);
	InspectionMarker = CreateSpriteComponent(TEXT("InspectionMarker"), 72, FLinearColor(0.45f, 0.82f, 0.54f, 0.55f));
	FuelMarker = CreateSpriteComponent(TEXT("FuelMarker"), 72, FLinearColor(0.32f, 0.83f, 0.91f, 0.55f));

	TerrainSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Surfaces/S_TemperateGrass.S_TemperateGrass"));
	RunwaySprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Surfaces/S_GrassRunway.S_GrassRunway"));
	TaxiSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Surfaces/S_TaxiWear.S_TaxiWear"));
	StandSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Surfaces/S_WarmStand.S_WarmStand"));
	WhiteSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Props/S_WhiteSquare.S_WhiteSquare"));
	HutSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Props/S_OperationsHut.S_OperationsHut"));
	WindsockSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Props/S_Windsock.S_Windsock"));
	SelectionSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Props/S_SelectionRing.S_SelectionRing"));

	for (int32 Index = 0; Index < 16; ++Index)
	{
		const FString Path = FString::Printf(
			TEXT("/Game/Phase1/Presentation/Sprites/Aircraft/S_Cessna152_Heading_%02d.S_Cessna152_Heading_%02d"),
			Index,
			Index);
		AircraftHeadingSprites.Add(FindSprite(*Path));
	}

	ConfigureSprite(Terrain, TerrainSprite, FVector(0.0, 0.0, 0.0), FVector(100.0, 1.0, 100.0));
	Terrain->SetSpriteColor(FLinearColor(0.25f, 0.34f, 0.22f, 1.0f));
	ConfigureSprite(Runway, RunwaySprite, FVector(-5000.0, 0.0, 10.0), FVector(2.5, 1.0, 45.0));
	ConfigureSprite(Taxiway, TaxiSprite, FVector(-20000.0, 18000.0, 20.0), FVector(10.0, 1.0, 1.25));
	ConfigureSprite(Stand, StandSprite, FVector(-32000.0, 23000.0, 30.0), FVector(10.0, 1.0, 8.0));
	ConfigureSprite(Access, TaxiSprite, FVector(-35000.0, 36000.0, 20.0), FVector(7.0, 1.0, 1.25));
	ConfigureSprite(OperationsHut, HutSprite, FVector(-30000.0, 36000.0, 40.0), FVector(6.0, 1.0, 6.0));
	ConfigureSprite(Windsock, WindsockSprite, FVector(14000.0, -45000.0, 45.0), FVector(3.0, 1.0, 3.0));
	Aircraft->SetRelativeScale3D(FVector(10.0, 1.0, 10.0));
	ConfigureSprite(Selection, SelectionSprite, FVector(-32000.0, 23000.0, 70.0), FVector(2.2, 1.0, 2.2));
	ConfigureSprite(InspectionMarker, SelectionSprite, FVector(-32000.0, 18000.0, 72.0), FVector(1.1, 1.0, 1.1));
	ConfigureSprite(FuelMarker, SelectionSprite, FVector(-32000.0, 28000.0, 72.0), FVector(1.1, 1.0, 1.1));

	SetFacilitiesVisible(false, false);
	Aircraft->SetVisibility(false);
	Selection->SetVisibility(false);
	InspectionMarker->SetVisibility(false);
	FuelMarker->SetVisibility(false);
}

UPaperSpriteComponent* AAMSimWorldPresenter::CreateSpriteComponent(
	const TCHAR* Name,
	const int32 SortPriority,
	const FLinearColor& Color)
{
	UPaperSpriteComponent* Component = CreateDefaultSubobject<UPaperSpriteComponent>(Name);
	Component->SetupAttachment(Root);
	Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Component->SetGenerateOverlapEvents(false);
	Component->SetCastShadow(false);
	Component->SetTranslucentSortPriority(SortPriority);
	Component->SetSpriteColor(Color);
	Component->SetRelativeRotation(FRotator(0.0f, 0.0f, SpritePlaneRoll));
	return Component;
}

void AAMSimWorldPresenter::ConfigureSprite(
	UPaperSpriteComponent* Component,
	UPaperSprite* Sprite,
	const FVector& Location,
	const FVector& Scale)
{
	Component->SetSprite(Sprite);
	Component->SetRelativeLocation(Location);
	Component->SetRelativeScale3D(Scale);
}

bool AAMSimWorldPresenter::HasRequiredPresentationAssets() const
{
	return TerrainSprite && RunwaySprite && TaxiSprite && StandSprite && WhiteSprite &&
		HutSprite && WindsockSprite && SelectionSprite &&
		AircraftHeadingSprites.Num() == 16 &&
		Algo::AllOf(AircraftHeadingSprites, [](const UPaperSprite* Sprite) { return Sprite != nullptr; });
}

int32 AAMSimWorldPresenter::GetHeadingIndex(const AMSim::EFlightState FlightState)
{
	const float Degrees = AMSim::GetPhase1AircraftPresentationHeadingDegrees(FlightState);
	return FMath::RoundToInt(FMath::Fmod(Degrees + 360.0f, 360.0f) / 22.5f) % 16;
}

void AAMSimWorldPresenter::ApplySnapshot(const AMSim::FPhase1QuerySnapshot& Query)
{
	if (LastAppliedRevision == Query.Revision)
	{
		return;
	}
	LastAppliedRevision = Query.Revision;
	const bool bFacilitiesVisible = Query.ConstructionStage != AMSim::EConstructionStage::None;
	const bool bOperational = Query.ConstructionStage >= AMSim::EConstructionStage::ReadyToOpen;
	SetFacilitiesVisible(bFacilitiesVisible, bOperational);
	SetAircraftState(Query);
}

void AAMSimWorldPresenter::SetFacilitiesVisible(const bool bVisible, const bool bOperational)
{
	for (UPaperSpriteComponent* Component : {Runway, Taxiway, Stand, Access, OperationsHut, Windsock})
	{
		Component->SetVisibility(bVisible);
	}
	const FLinearColor ProposalTint(0.24f, 0.68f, 0.74f, 0.52f);
	Runway->SetSpriteColor(bOperational ? FLinearColor::White : ProposalTint);
	Taxiway->SetSpriteColor(bOperational ? FLinearColor::White : ProposalTint);
	Stand->SetSpriteColor(bOperational ? FLinearColor::White : ProposalTint);
	Access->SetSpriteColor(bOperational ? FLinearColor::White : ProposalTint);
	OperationsHut->SetSpriteColor(bOperational ? FLinearColor::White : ProposalTint);
	Windsock->SetSpriteColor(bOperational ? FLinearColor::White : ProposalTint);
}

void AAMSimWorldPresenter::SetAircraftState(const AMSim::FPhase1QuerySnapshot& Query)
{
	const bool bVisible =
		Query.FlightState != AMSim::EFlightState::None &&
		Query.FlightState != AMSim::EFlightState::Completed;
	Aircraft->SetVisibility(bVisible);
	Selection->SetVisibility(bVisible);
	if (!bVisible)
	{
		InspectionMarker->SetVisibility(false);
		FuelMarker->SetVisibility(false);
		return;
	}

	const int32 HeadingIndex = GetHeadingIndex(Query.FlightState);
	if (AircraftHeadingSprites.IsValidIndex(HeadingIndex))
	{
		Aircraft->SetSprite(AircraftHeadingSprites[HeadingIndex]);
	}

	FVector Position(18000.0, -42000.0, 60.0);
	if (Query.FlightState >= AMSim::EFlightState::Approach &&
		Query.FlightState <= AMSim::EFlightState::RunwayRoll)
	{
		Position = FVector(-5000.0, 0.0, 60.0);
	}
	else if (Query.FlightState >= AMSim::EFlightState::TaxiIn &&
		Query.FlightState <= AMSim::EFlightState::Ready)
	{
		Position = FVector(-32000.0, 23000.0, 60.0);
	}
	else if (Query.FlightState >= AMSim::EFlightState::TaxiOut)
	{
		Position = FVector(-18000.0, 15000.0, 60.0);
	}
	Aircraft->SetRelativeLocation(Position);
	Selection->SetRelativeLocation(FVector(Position.X, Position.Y, 70.0));

	const bool bTurnaround = Query.FlightState == AMSim::EFlightState::Turnaround;
	InspectionMarker->SetVisibility(bTurnaround && Query.InspectionState == AMSim::EServiceTaskState::Active);
	FuelMarker->SetVisibility(bTurnaround && Query.FuelingState == AMSim::EServiceTaskState::Active);
}
