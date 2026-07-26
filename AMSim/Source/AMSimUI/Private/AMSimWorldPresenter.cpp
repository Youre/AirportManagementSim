#include "AMSimWorldPresenter.h"

#include "Algo/AllOf.h"
#include "Algo/Count.h"
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
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Phase2Aircraft.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase2Aircraft%d"), Index),
			61 + Index));
		Phase2Vehicles.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase2Vehicle%d"), Index),
			64 + Index,
			FLinearColor(0.95f, 0.65f, 0.18f, 0.9f)));
	}
	ExpansionOverlay = CreateSpriteComponent(
		TEXT("Phase2ExpansionOverlay"),
		35,
		FLinearColor(0.25f, 0.78f, 0.86f, 0.38f));
	IncidentOverlay = CreateSpriteComponent(
		TEXT("Phase2IncidentOverlay"),
		80,
		FLinearColor(0.95f, 0.25f, 0.20f, 0.65f));
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Phase3Floor.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3Floor%d"), Index),
			22 + Index,
			FLinearColor(0.02f, 0.09f, 0.10f, 1.0f)));
	}
	for (int32 Index = 0; Index < 10; ++Index)
	{
		Phase3Rooms.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3Room%d"), Index),
			25 + Index,
			FLinearColor(0.11f, 0.19f, 0.20f, 1.0f)));
	}
	for (int32 Index = 0; Index < 6; ++Index)
	{
		Phase3DepartureFlow.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3DepartureFlow%d"), Index),
			52,
			FLinearColor(0.24f, 0.83f, 0.91f, 0.74f)));
		Phase3BaggageFlow.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3BaggageFlow%d"), Index),
			51,
			FLinearColor(0.91f, 0.55f, 0.18f, 0.68f)));
	}
	for (int32 Index = 0; Index < 5; ++Index)
	{
		Phase3ArrivalFlow.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3ArrivalFlow%d"), Index),
			53,
			FLinearColor(0.76f, 0.42f, 0.92f, 0.72f)));
		Phase3AccessibleFlow.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3AccessibleFlow%d"), Index),
			56,
			FLinearColor(0.96f, 0.82f, 0.27f, 0.92f)));
	}
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Phase3LandsideFlow.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3LandsideFlow%d"), Index),
			50,
			FLinearColor(0.32f, 0.75f, 0.38f, 0.76f)));
	}
	for (int32 Index = 0; Index < 8; ++Index)
	{
		Phase3SecurityBoundary.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3SecurityBoundary%d"), Index),
			49,
			FLinearColor(0.20f, 0.58f, 0.72f, 0.80f)));
	}
	for (int32 Index = 0; Index < 3; ++Index)
	{
		Phase3Congestion.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3Congestion%d"), Index),
			48,
			FLinearColor(0.96f, 0.58f, 0.14f, 0.44f)));
	}
	for (int32 Index = 0; Index < 64; ++Index)
	{
		Phase3Passengers.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3Passenger%d"), Index),
			70 + (Index % 4),
			FLinearColor(0.94f, 0.96f, 0.93f, 1.0f)));
	}
	for (int32 Index = 0; Index < 36; ++Index)
	{
		Phase3Bags.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3Bag%d"), Index),
			65,
			FLinearColor(0.96f, 0.63f, 0.20f, 1.0f)));
	}
	for (int32 Index = 0; Index < 6; ++Index)
	{
		Phase3Vehicles.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3Vehicle%d"), Index),
			58 + Index,
			Index == 5
				? FLinearColor(0.25f, 0.70f, 0.86f, 1.0f)
				: FLinearColor(0.88f, 0.91f, 0.88f, 1.0f)));
	}
	for (int32 Index = 0; Index < 12; ++Index)
	{
		Phase3Staff.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3Staff%d"), Index),
			74,
			Index < 4
				? FLinearColor(0.30f, 0.84f, 0.94f, 1.0f)
				: Index < 8
					? FLinearColor(0.36f, 0.78f, 0.45f, 1.0f)
					: FLinearColor(0.97f, 0.62f, 0.22f, 1.0f)));
	}
	Phase3Aircraft = CreateSpriteComponent(
		TEXT("Phase3Aircraft"),
		67,
		FLinearColor(0.92f, 0.58f, 0.22f, 1.0f));

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
	for (int32 Index = 0; Index < Phase2Aircraft.Num(); ++Index)
	{
		ConfigureSprite(
			Phase2Aircraft[Index],
			AircraftHeadingSprites.IsValidIndex(Index * 4)
				? AircraftHeadingSprites[Index * 4]
				: WhiteSprite,
			FVector(-32000.0, 23000.0 + Index * 8000.0, 62.0 + Index),
			FVector(8.0, 1.0, 8.0));
		ConfigureSprite(
			Phase2Vehicles[Index],
			WhiteSprite,
			FVector(-39000.0, 20000.0 + Index * 6000.0, 64.0 + Index),
			FVector(1.8, 1.0, 3.0));
		Phase2Aircraft[Index]->SetVisibility(false);
		Phase2Vehicles[Index]->SetVisibility(false);
	}
	const FVector RoomLocations[] = {
		FVector(-27000.0, -25000.0, 25.0),
		FVector(-2000.0, -24000.0, 26.0),
		FVector(0.0, -3000.0, 27.0),
		FVector(16000.0, -7000.0, 28.0),
		FVector(27000.0, -20000.0, 29.0),
		FVector(-11000.0, 11000.0, 30.0),
		FVector(5000.0, 16000.0, 31.0),
		FVector(-16000.0, -10000.0, 32.0),
		FVector(27000.0, 7000.0, 33.0),
		FVector(-35000.0, 9000.0, 34.0)};
	const FVector FloorLocations[] = {
		FVector(-1000.0, -5000.0, 22.0),
		FVector(21000.0, -7000.0, 23.0),
		FVector(-21000.0, -9000.0, 24.0),
		FVector(-35000.0, -2000.0, 25.0)};
	const FVector FloorScales[] = {
		FVector(225.0, 1.0, 210.0),
		FVector(125.0, 1.0, 135.0),
		FVector(115.0, 1.0, 145.0),
		FVector(18.0, 1.0, 330.0)};
	for (int32 Index = 0; Index < Phase3Floor.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3Floor[Index],
			WhiteSprite,
			FloorLocations[Index],
			FloorScales[Index]);
		Phase3Floor[Index]->SetSpriteColor(
			Index == 0
				? FLinearColor(0.20f, 0.23f, 0.21f, 1.0f)
				: Index == 1
					? FLinearColor(0.07f, 0.20f, 0.23f, 1.0f)
					: Index == 2
						? FLinearColor(0.24f, 0.22f, 0.17f, 1.0f)
						: FLinearColor(0.09f, 0.11f, 0.11f, 1.0f));
	}
	const FVector RoomScales[] = {
		FVector(70.0, 1.0, 42.0),
		FVector(95.0, 1.0, 55.0),
		FVector(68.0, 1.0, 55.0),
		FVector(125.0, 1.0, 70.0),
		FVector(72.0, 1.0, 48.0),
		FVector(105.0, 1.0, 38.0),
		FVector(72.0, 1.0, 55.0),
		FVector(95.0, 1.0, 50.0),
		FVector(72.0, 1.0, 48.0),
		FVector(135.0, 1.0, 24.0)};
	for (int32 Index = 0; Index < Phase3Rooms.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3Rooms[Index],
			WhiteSprite,
			RoomLocations[Index],
			RoomScales[Index]);
		const FLinearColor RoomPalette[] = {
			FLinearColor(0.30f, 0.34f, 0.27f, 1.0f),
			FLinearColor(0.42f, 0.36f, 0.24f, 1.0f),
			FLinearColor(0.16f, 0.38f, 0.42f, 1.0f),
			FLinearColor(0.20f, 0.43f, 0.47f, 1.0f),
			FLinearColor(0.17f, 0.39f, 0.46f, 1.0f),
			FLinearColor(0.35f, 0.23f, 0.43f, 1.0f),
			FLinearColor(0.45f, 0.32f, 0.15f, 1.0f),
			FLinearColor(0.37f, 0.25f, 0.46f, 1.0f),
			FLinearColor(0.25f, 0.38f, 0.30f, 1.0f),
			FLinearColor(0.20f, 0.23f, 0.23f, 1.0f)};
		Phase3Rooms[Index]->SetSpriteColor(RoomPalette[Index]);
	}
	const FVector DeparturePoints[] = {
		FVector(-27000.0, -25000.0, 52.0),
		FVector(-12000.0, -24000.0, 52.0),
		FVector(0.0, -15000.0, 52.0),
		FVector(5000.0, -3000.0, 52.0),
		FVector(17000.0, -8000.0, 52.0),
		FVector(27000.0, -20000.0, 52.0)};
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
	for (int32 Index = 0; Index < Phase3DepartureFlow.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3DepartureFlow[Index],
			WhiteSprite,
			DeparturePoints[Index],
			FVector(Index % 2 == 0 ? 45.0 : 75.0, 1.0, 3.2));
		ConfigureSprite(
			Phase3BaggageFlow[Index],
			WhiteSprite,
			BaggagePoints[Index],
			FVector(54.0, 1.0, 2.7));
	}
	for (int32 Index = 0; Index < Phase3ArrivalFlow.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3ArrivalFlow[Index],
			WhiteSprite,
			ArrivalPoints[Index],
			FVector(58.0, 1.0, 3.2));
		ConfigureSprite(
			Phase3AccessibleFlow[Index],
			WhiteSprite,
			DeparturePoints[Index],
			FVector(26.0, 1.0, 1.3));
	}
	for (int32 Index = 0; Index < Phase3LandsideFlow.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3LandsideFlow[Index],
			WhiteSprite,
			FVector(-35000.0, -33000.0 + Index * 18000.0, 50.0),
			FVector(66.0, 1.0, 3.2));
	}
	for (int32 Index = 0; Index < Phase3SecurityBoundary.Num(); ++Index)
	{
		const bool bVertical = Index % 2 == 0;
		ConfigureSprite(
			Phase3SecurityBoundary[Index],
			WhiteSprite,
			FVector(
				-9000.0 + (Index / 2) * 9000.0,
				-2000.0 + (Index % 2) * 18000.0,
				49.0),
			bVertical
				? FVector(1.2, 1.0, 58.0)
				: FVector(48.0, 1.0, 1.2));
	}
	for (int32 Index = 0; Index < Phase3Congestion.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3Congestion[Index],
			WhiteSprite,
			FVector(-15000.0 + Index * 11000.0, 4000.0, 48.0),
			FVector(34.0, 1.0, 18.0));
	}
	for (int32 Index = 0; Index < Phase3Passengers.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3Passengers[Index],
			WhiteSprite,
			FVector::ZeroVector,
			FVector(3.2, 1.0, 3.2));
	}
	for (int32 Index = 0; Index < Phase3Bags.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3Bags[Index],
			WhiteSprite,
			FVector::ZeroVector,
			FVector(2.4, 1.0, 1.8));
	}
	for (int32 Index = 0; Index < Phase3Vehicles.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3Vehicles[Index],
			WhiteSprite,
			FVector(-35000.0, -31000.0 + Index * 13000.0, 58.0 + Index),
			Index == 5
				? FVector(28.0, 1.0, 8.0)
				: FVector(14.0, 1.0, 7.0));
	}
	for (int32 Index = 0; Index < Phase3Staff.Num(); ++Index)
	{
		const FVector WorkArea =
			Index < 4
				? FVector(0.0, -3000.0, 74.0)
				: Index < 8
					? FVector(-2000.0, -24000.0, 74.0)
					: FVector(5000.0, 16000.0, 74.0);
		ConfigureSprite(
			Phase3Staff[Index],
			WhiteSprite,
			WorkArea + FVector(
				(Index % 4) * 900.0,
				(Index % 2) * 900.0,
				0.0),
			FVector(2.0, 1.0, 2.8));
	}
	ConfigureSprite(
		Phase3Aircraft,
		AircraftHeadingSprites.IsValidIndex(0)
			? AircraftHeadingSprites[0]
			: WhiteSprite,
		FVector(33000.0, -22000.0, 67.0),
		FVector(15.0, 1.0, 15.0));
	ConfigureSprite(
		ExpansionOverlay,
		StandSprite,
		FVector(-35000.0, 38000.0, 35.0),
		FVector(10.0, 1.0, 8.0));
	ConfigureSprite(
		IncidentOverlay,
		SelectionSprite,
		FVector(-32000.0, 23000.0, 80.0),
		FVector(3.2, 1.0, 3.2));
	ExpansionOverlay->SetVisibility(false);
	IncidentOverlay->SetVisibility(false);

	SetFacilitiesVisible(false, false);
	Aircraft->SetVisibility(false);
	Selection->SetVisibility(false);
	InspectionMarker->SetVisibility(false);
	FuelMarker->SetVisibility(false);
	SetPhase3WorldVisible(false);
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

int32 AAMSimWorldPresenter::GetPhase2HeadingIndex(
	const AMSim::EPhase2FlightState FlightState)
{
	switch (FlightState)
	{
	case AMSim::EPhase2FlightState::Inbound:
	case AMSim::EPhase2FlightState::Approach:
	case AMSim::EPhase2FlightState::Landing:
	case AMSim::EPhase2FlightState::TaxiIn:
		return 10;
	case AMSim::EPhase2FlightState::ReadyToDepart:
	case AMSim::EPhase2FlightState::TaxiOut:
	case AMSim::EPhase2FlightState::Outbound:
		return 2;
	default:
		return 0;
	}
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

void AAMSimWorldPresenter::ApplyPhase2Snapshot(
	const AMSim::FPhase2QuerySnapshot& Query,
	const AMSim::FPhase2State& State)
{
	if (LastAppliedPhase2Revision == Query.Revision)
	{
		return;
	}
	LastAppliedPhase2Revision = Query.Revision;

	int32 ProxyIndex = 0;
	for (const AMSim::FPhase2FlightRecord& Flight : State.Flights)
	{
		if (Flight.State == AMSim::EPhase2FlightState::Completed ||
			Flight.State == AMSim::EPhase2FlightState::Cancelled ||
			ProxyIndex >= Phase2Aircraft.Num())
		{
			continue;
		}
		UPaperSpriteComponent* Proxy = Phase2Aircraft[ProxyIndex];
		const AMSim::FPhase2AircraftRecord* AircraftRecord =
			State.Aircraft.FindByPredicate(
				[&Flight](const AMSim::FPhase2AircraftRecord& Candidate)
					{
						return Candidate.Id == Flight.AircraftId;
					});
		float RoleScale = 10.0f;
		FLinearColor RoleColor = FLinearColor(0.90f, 0.95f, 0.96f, 1.0f);
		if (AircraftRecord &&
			AircraftRecord->RoleId == TEXT("AircraftRole.BasicTrainer"))
		{
			RoleScale = 9.0f;
			RoleColor = FLinearColor(0.32f, 0.83f, 0.91f, 1.0f);
		}
		else if (AircraftRecord &&
			AircraftRecord->RoleId == TEXT("AircraftRole.BusinessTurboprop"))
		{
			RoleScale = 14.0f;
			RoleColor = FLinearColor(0.95f, 0.65f, 0.18f, 1.0f);
		}
		else if (AircraftRecord &&
			AircraftRecord->RoleId == TEXT("AircraftRole.LightBusinessJet"))
		{
			RoleScale = 13.0f;
			RoleColor = FLinearColor(0.95f, 0.45f, 0.38f, 1.0f);
		}
		Proxy->SetRelativeScale3D(FVector(RoleScale, 1.0, RoleScale));
		Proxy->SetSpriteColor(RoleColor);
		const int32 HeadingIndex = GetPhase2HeadingIndex(Flight.State);
		if (AircraftHeadingSprites.IsValidIndex(HeadingIndex))
		{
			Proxy->SetSprite(AircraftHeadingSprites[HeadingIndex]);
		}
		FVector Position(
			18000.0 - ProxyIndex * 6000.0,
			-42000.0 + ProxyIndex * 8000.0,
			62.0 + ProxyIndex);
		if (Flight.State >= AMSim::EPhase2FlightState::Landing &&
			Flight.State <= AMSim::EPhase2FlightState::TaxiIn)
		{
			Position = FVector(
				-5000.0 + ProxyIndex * 8000.0,
				ProxyIndex * 2500.0,
				62.0 + ProxyIndex);
		}
		else if (Flight.State >= AMSim::EPhase2FlightState::Turnaround &&
			Flight.State <= AMSim::EPhase2FlightState::ReadyToDepart)
		{
			Position = FVector(
				-32000.0,
				23000.0 + ProxyIndex * 8000.0,
				62.0 + ProxyIndex);
		}
		else if (Flight.State >= AMSim::EPhase2FlightState::TaxiOut)
		{
			Position = FVector(
				-18000.0 + ProxyIndex * 5000.0,
				15000.0 + ProxyIndex * 3000.0,
				62.0 + ProxyIndex);
		}
		Proxy->SetRelativeLocation(Position);
		Proxy->SetVisibility(Query.bInitialized);
		++ProxyIndex;
	}
	for (; ProxyIndex < Phase2Aircraft.Num(); ++ProxyIndex)
	{
		Phase2Aircraft[ProxyIndex]->SetVisibility(false);
	}

	ProxyIndex = 0;
	for (const AMSim::FPhase2VehicleRecord& Vehicle : State.Vehicles)
	{
		if (Vehicle.State == AMSim::EPhase2VehicleState::AtDepot ||
			ProxyIndex >= Phase2Vehicles.Num())
		{
			continue;
		}
		Phase2Vehicles[ProxyIndex]->SetRelativeLocation(FVector(
			-38500.0,
			20500.0 + ProxyIndex * 6500.0,
			66.0 + ProxyIndex));
		Phase2Vehicles[ProxyIndex]->SetVisibility(Query.bInitialized);
		++ProxyIndex;
	}
	for (; ProxyIndex < Phase2Vehicles.Num(); ++ProxyIndex)
	{
		Phase2Vehicles[ProxyIndex]->SetVisibility(false);
	}

	ExpansionOverlay->SetVisibility(
		Query.bInitialized &&
		State.Expansion.Stage != AMSim::EExpansionStage::None);
	ExpansionOverlay->SetSpriteColor(
		State.Expansion.Stage == AMSim::EExpansionStage::Operational
			? FLinearColor::White
			: FLinearColor(0.25f, 0.78f, 0.86f, 0.38f));
	IncidentOverlay->SetVisibility(
		Query.bInitialized &&
		State.Incident.Id.IsValid() &&
		State.Incident.State != AMSim::EIncidentState::Resolved);
	Terrain->SetSpriteColor(
		Query.WeatherCategory == AMSim::EWeatherCategory::ColdWet
			? FLinearColor(0.55f, 0.62f, 0.55f, 1.0f)
			: Query.WeatherCategory == AMSim::EWeatherCategory::Wet
				? FLinearColor(0.22f, 0.34f, 0.28f, 1.0f)
				: FLinearColor(0.25f, 0.34f, 0.22f, 1.0f));
}

void AAMSimWorldPresenter::ApplyPhase3Snapshot(
	const AMSim::FPhase3QuerySnapshot& Query,
	const AMSim::FPhase3State& State)
{
	if (LastAppliedPhase3Revision == Query.Revision)
	{
		return;
	}
	LastAppliedPhase3Revision = Query.Revision;
	const bool bShowWorld =
		Query.bInitialized &&
		Query.TerminalStage >= AMSim::ETerminalConstructionStage::ShellReady;
	bPhase3RoutesConnected =
		Query.RequiredConnectionCount > 0 &&
		Query.ConnectedCount == Query.RequiredConnectionCount;
	SetPhase3WorldVisible(bShowWorld);
	if (!bShowWorld)
	{
		return;
	}

	SetFacilitiesVisible(false, false);
	Aircraft->SetVisibility(false);
	Selection->SetVisibility(false);
	InspectionMarker->SetVisibility(false);
	FuelMarker->SetVisibility(false);
	for (UPaperSpriteComponent* Component : Phase2Aircraft)
	{
		Component->SetVisibility(false);
	}
	for (UPaperSpriteComponent* Component : Phase2Vehicles)
	{
		Component->SetVisibility(false);
	}
	ExpansionOverlay->SetVisibility(false);
	IncidentOverlay->SetVisibility(false);
	Terrain->SetSpriteColor(FLinearColor(0.20f, 0.30f, 0.23f, 1.0f));

	int32 PassengerProxyIndex = 0;
	for (const AMSim::FPassengerRecord& Passenger : State.Passengers)
	{
		if (PassengerProxyIndex >= Phase3Passengers.Num())
		{
			break;
		}
		FVector Base;
		switch (Passenger.JourneyState)
		{
		case AMSim::EPassengerJourneyState::ApproachingAirport:
			Base = FVector(-35000.0, -30000.0, 72.0);
			break;
		case AMSim::EPassengerJourneyState::LandsideEntry:
			Base = FVector(-27000.0, -25000.0, 72.0);
			break;
		case AMSim::EPassengerJourneyState::CheckInBagDrop:
			Base = FVector(-2000.0, -24000.0, 72.0);
			break;
		case AMSim::EPassengerJourneyState::SecurityQueue:
		case AMSim::EPassengerJourneyState::Screening:
			Base = FVector(0.0, -3000.0, 72.0);
			break;
		case AMSim::EPassengerJourneyState::GateArea:
		case AMSim::EPassengerJourneyState::Boarding:
			Base = FVector(19000.0, -13000.0, 72.0);
			break;
		case AMSim::EPassengerJourneyState::OnAircraft:
			Base = FVector(30000.0, -22000.0, 72.0);
			break;
		case AMSim::EPassengerJourneyState::ArrivalsCorridor:
			Base = FVector(-11000.0, 11000.0, 72.0);
			break;
		case AMSim::EPassengerJourneyState::BaggageReclaim:
			Base = FVector(-16000.0, -10000.0, 72.0);
			break;
		case AMSim::EPassengerJourneyState::GroundTransport:
		case AMSim::EPassengerJourneyState::Completed:
			Base = FVector(-30000.0, -30000.0, 72.0);
			break;
		default:
			Base = FVector(-5000.0, 5000.0, 72.0);
			break;
		}
		const int32 LocalIndex = PassengerProxyIndex;
		Base.X += (LocalIndex % 8) * 900.0;
		Base.Y += ((LocalIndex / 8) % 8) * 900.0;
		UPaperSpriteComponent* Proxy = Phase3Passengers[PassengerProxyIndex];
		Proxy->SetRelativeLocation(Base);
		Proxy->SetSpriteColor(
			Passenger.bRequiresAccessibleRoute
				? FLinearColor(0.98f, 0.82f, 0.25f, 1.0f)
				: Passenger.Direction == AMSim::EPassengerDirection::Departing
					? FLinearColor(0.74f, 0.94f, 0.96f, 1.0f)
					: FLinearColor(0.83f, 0.66f, 0.96f, 1.0f));
		Proxy->SetVisibility(true);
		++PassengerProxyIndex;
	}
	for (; PassengerProxyIndex < Phase3Passengers.Num(); ++PassengerProxyIndex)
	{
		Phase3Passengers[PassengerProxyIndex]->SetVisibility(false);
	}

	int32 BagProxyIndex = 0;
	for (const AMSim::FBagRecord& Bag : State.Bags)
	{
		if (BagProxyIndex >= Phase3Bags.Num())
		{
			break;
		}
		FVector Base;
		switch (Bag.JourneyState)
		{
		case AMSim::EBagJourneyState::Accepted:
		case AMSim::EBagJourneyState::Conveyor:
			Base = FVector(-2000.0, -19000.0, 65.0);
			break;
		case AMSim::EBagJourneyState::Screened:
		case AMSim::EBagJourneyState::Sorted:
			Base = FVector(5000.0, 8000.0, 65.0);
			break;
		case AMSim::EBagJourneyState::MakeUp:
		case AMSim::EBagJourneyState::OnAircraft:
			Base = FVector(15000.0, 15000.0, 65.0);
			break;
		case AMSim::EBagJourneyState::ArrivalInfeed:
		case AMSim::EBagJourneyState::Reclaim:
		case AMSim::EBagJourneyState::Collected:
			Base = FVector(-16000.0, -10000.0, 65.0);
			break;
		default:
			Base = FVector(5000.0, 18000.0, 65.0);
			break;
		}
		Base.X += (BagProxyIndex % 9) * 600.0;
		Base.Y += ((BagProxyIndex / 9) % 4) * 560.0;
		UPaperSpriteComponent* Proxy = Phase3Bags[BagProxyIndex];
		Proxy->SetRelativeLocation(Base);
		Proxy->SetSpriteColor(
			Bag.JourneyState == AMSim::EBagJourneyState::Exception
				? FLinearColor(0.95f, 0.20f, 0.16f, 1.0f)
				: FLinearColor(0.96f, 0.63f, 0.20f, 1.0f));
		Proxy->SetVisibility(true);
		++BagProxyIndex;
	}
	for (; BagProxyIndex < Phase3Bags.Num(); ++BagProxyIndex)
	{
		Phase3Bags[BagProxyIndex]->SetVisibility(false);
	}
	Phase3Aircraft->SetVisibility(
		State.Flight.Id.IsValid() &&
		State.Flight.State != AMSim::EPhase3FlightState::Completed);
	RefreshPhase3OverlayVisibility();
}

void AAMSimWorldPresenter::SetPhase3OverlayMode(const int32 Mode)
{
	Phase3OverlayMode = FMath::Clamp(Mode, 0, 4);
	RefreshPhase3OverlayVisibility();
}

void AAMSimWorldPresenter::SetPhase3WorldVisible(const bool bVisible)
{
	bPhase3WorldVisible = bVisible;
	for (UPaperSpriteComponent* Component : Phase3Floor)
	{
		Component->SetVisibility(bVisible);
	}
	for (UPaperSpriteComponent* Component : Phase3Rooms)
	{
		Component->SetVisibility(bVisible);
	}
	for (UPaperSpriteComponent* Component : Phase3SecurityBoundary)
	{
		Component->SetVisibility(bVisible);
	}
	for (UPaperSpriteComponent* Component : Phase3Congestion)
	{
		Component->SetVisibility(false);
	}
	for (UPaperSpriteComponent* Component : Phase3Passengers)
	{
		Component->SetVisibility(false);
	}
	for (UPaperSpriteComponent* Component : Phase3Bags)
	{
		Component->SetVisibility(false);
	}
	for (UPaperSpriteComponent* Component : Phase3Vehicles)
	{
		Component->SetVisibility(bVisible);
	}
	for (UPaperSpriteComponent* Component : Phase3Staff)
	{
		Component->SetVisibility(bVisible);
	}
	Phase3Aircraft->SetVisibility(false);
	RefreshPhase3OverlayVisibility();
}

void AAMSimWorldPresenter::RefreshPhase3OverlayVisibility()
{
	const bool bRouteReady = bPhase3WorldVisible && bPhase3RoutesConnected;
	const bool bAll = Phase3OverlayMode == 0;
	const auto SetFamilyVisible =
		[bRouteReady](const TArray<TObjectPtr<UPaperSpriteComponent>>& Family,
			const bool bVisible)
		{
			for (UPaperSpriteComponent* Component : Family)
			{
				Component->SetVisibility(bRouteReady && bVisible);
			}
		};
	SetFamilyVisible(
		Phase3DepartureFlow,
		bAll || Phase3OverlayMode == 1 || Phase3OverlayMode == 4);
	SetFamilyVisible(
		Phase3ArrivalFlow,
		bAll || Phase3OverlayMode == 2);
	SetFamilyVisible(
		Phase3LandsideFlow,
		bAll || Phase3OverlayMode == 1 || Phase3OverlayMode == 2 ||
			Phase3OverlayMode == 4);
	SetFamilyVisible(
		Phase3BaggageFlow,
		bAll || Phase3OverlayMode == 3);
	SetFamilyVisible(
		Phase3AccessibleFlow,
		bAll || Phase3OverlayMode == 4);
	for (UPaperSpriteComponent* Component : Phase3Congestion)
	{
		Component->SetVisibility(bRouteReady && (bAll || Phase3OverlayMode == 1));
	}
}

int32 AAMSimWorldPresenter::GetActivePhase2AircraftProxyCount() const
{
	return Algo::CountIf(
		Phase2Aircraft,
		[](const UPaperSpriteComponent* Component)
			{
				return Component && Component->IsVisible();
			});
}

int32 AAMSimWorldPresenter::GetActivePhase2VehicleProxyCount() const
{
	return Algo::CountIf(
		Phase2Vehicles,
		[](const UPaperSpriteComponent* Component)
			{
				return Component && Component->IsVisible();
			});
}

int32 AAMSimWorldPresenter::GetActivePhase3PassengerProxyCount() const
{
	return Algo::CountIf(
		Phase3Passengers,
		[](const UPaperSpriteComponent* Component)
			{
				return Component && Component->IsVisible();
			});
}

int32 AAMSimWorldPresenter::GetActivePhase3BagProxyCount() const
{
	return Algo::CountIf(
		Phase3Bags,
		[](const UPaperSpriteComponent* Component)
			{
				return Component && Component->IsVisible();
			});
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
