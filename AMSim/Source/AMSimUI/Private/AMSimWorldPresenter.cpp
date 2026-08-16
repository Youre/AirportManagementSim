#include "AMSimWorldPresenter.h"

#include "Algo/AllOf.h"
#include "Algo/Count.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimProceduralSurfaceComponent.h"
#include "AMSimWorldPresentationLayers.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "UObject/ConstructorHelpers.h"
namespace AMSimWorldPresenterPrivate
{
	UPaperSprite* FindSprite(const TCHAR* ObjectPath)
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperSprite> Finder(ObjectPath);
		return Finder.Get();
	}

	constexpr float SpritePlaneRoll = -90.0f;

	enum class EOperationsSprite : int32
	{
		FuelTruck,
		BaggageTug,
		BaggageCartTrain,
		OperationsVan,
		FireRescueTruck,
		ConstructionTruck,
		ShuttleBus,
		Taxi,
		RentalCar,
		Deicer,
		RampWorker,
		ConstructionWorker,
		SecurityOfficer,
		TerminalAgent,
		PassengerFamily,
		Passenger,
		Suitcase,
		SafetyCones,
		PortableStairs,
		PushbackTug,
		Count
	};

	enum class ETerminalSprite : int32
	{
		CheckInDesk,
		BagDropDesk,
		SecurityScanner,
		QueueBarriers,
		GatePodium,
		SeatingCluster,
		BaggageConveyor,
		SortingTable,
		BaggageCart,
		ReclaimCarousel,
		InformationDesk,
		RestroomBlock,
		EntranceDoors,
		SecureDoor,
		PartitionWall,
		CurbsideShelter,
		BusStopShelter,
		ParkingKiosk,
		TerminalFloor,
		SecureFloor,
		CautionHatch,
		DirectionArrow,
		AccessibleRoute,
		ProtectionZone,
		Count
	};

	enum class ESiteSprite : int32
	{
		RunwayAsphalt,
		TaxiwayAsphalt,
		ApronStand,
		AccessRoad,
		RegionalTerminal,
		GAHangars,
		OperationsStation,
		FuelFarm,
		ParkingLot,
		BusTaxiBay,
		RailPlatform,
		DropoffIsland,
		TreeCluster,
		LandscapeCluster,
		PerimeterGate,
		ApronFixtures,
		Count
	};
}

using namespace AMSimWorldPresenterPrivate;

AAMSimWorldPresenter::AAMSimWorldPresenter()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Terrain = CreateSpriteComponent(
		TEXT("Terrain"),
		AMSim::WorldPresentationLayers::Terrain.SortPriority);
	Runway = CreateProceduralSurfaceComponent(
		TEXT("Runway"),
		AMSim::WorldPresentationLayers::Runway.SortPriority);
	Taxiway = CreateProceduralSurfaceComponent(
		TEXT("Taxiway"),
		AMSim::WorldPresentationLayers::Taxiway.SortPriority);
	Phase1TaxiwaySegments.Add(Taxiway);
	for (int32 Index = 1; Index < 8; ++Index)
	{
		Phase1TaxiwaySegments.Add(CreateProceduralSurfaceComponent(
			*FString::Printf(TEXT("Phase1TaxiwaySegment%d"), Index),
			AMSim::WorldPresentationLayers::Taxiway.SortPriority));
	}
	Stand = CreateProceduralSurfaceComponent(
		TEXT("Stand"),
		AMSim::WorldPresentationLayers::GateA.SortPriority);
	GateB = CreateProceduralSurfaceComponent(
		TEXT("GateB"),
		AMSim::WorldPresentationLayers::GateB.SortPriority);
	Access = CreateProceduralSurfaceComponent(
		TEXT("Access"),
		AMSim::WorldPresentationLayers::ServiceRoad.SortPriority);
	OperationsHut = CreateSpriteComponent(
		TEXT("OperationsHut"),
		AMSim::WorldPresentationLayers::Structure.SortPriority);
	Windsock = CreateSpriteComponent(TEXT("Windsock"), 45);
	RunwayStartNumber = CreateDefaultSubobject<UTextRenderComponent>(
		TEXT("RunwayStartNumber"));
	RunwayStartNumber->SetupAttachment(Root);
	RunwayStartNumber->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RunwayEndNumber = CreateDefaultSubobject<UTextRenderComponent>(
		TEXT("RunwayEndNumber"));
	RunwayEndNumber->SetupAttachment(Root);
	RunwayEndNumber->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Aircraft = CreateSpriteComponent(TEXT("Aircraft"), 60);
	Selection = CreateSpriteComponent(TEXT("Selection"), 70);
	InspectionMarker = CreateSpriteComponent(TEXT("InspectionMarker"), 72, FLinearColor(0.45f, 0.82f, 0.54f, 0.55f));
	FuelMarker = CreateSpriteComponent(TEXT("FuelMarker"), 72, FLinearColor(0.32f, 0.83f, 0.91f, 0.55f));
	TurnaroundFuelTruck =
		CreateSpriteComponent(TEXT("TurnaroundFuelTruck"), 73);
	TurnaroundRampWorker =
		CreateSpriteComponent(TEXT("TurnaroundRampWorker"), 74);
	TurnaroundSafetyCones =
		CreateSpriteComponent(TEXT("TurnaroundSafetyCones"), 71);
	TurnaroundSafetyZone =
		CreateSpriteComponent(TEXT("TurnaroundSafetyZone"), 68);
	for (int32 Index = 0; Index < 2; ++Index)
	{
		TurnaroundApproachPaths.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("TurnaroundApproachPath%d"), Index),
			69 + Index));
	}
	InitializePhase1ConstructionPresentation();
	InitializePhase1ConstructionPreviewPresentation();
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
	ExpansionClosureOverlay = CreateSpriteComponent(
		TEXT("Phase2ExpansionClosureOverlay"),
		37,
		FLinearColor(0.96f, 0.34f, 0.27f, 0.72f));
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
			FLinearColor(0.05f, 0.75f, 0.92f, 0.96f)));
		Phase3BaggageFlow.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3BaggageFlow%d"), Index),
			51,
			FLinearColor(0.95f, 0.45f, 0.05f, 0.94f)));
	}
	for (int32 Index = 0; Index < 5; ++Index)
	{
		Phase3ArrivalFlow.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3ArrivalFlow%d"), Index),
			53,
			FLinearColor(0.67f, 0.25f, 0.90f, 0.94f)));
		Phase3AccessibleFlow.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3AccessibleFlow%d"), Index),
			56,
			FLinearColor(0.98f, 0.75f, 0.08f, 0.98f)));
	}
	for (int32 Index = 0; Index < 20; ++Index)
	{
		Phase3AccessibleDashes.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3AccessibleDash%d"), Index),
			57,
			FLinearColor(1.0f, 0.88f, 0.12f, 1.0f)));
	}
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Phase3LandsideFlow.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3LandsideFlow%d"), Index),
			50,
			FLinearColor(0.12f, 0.72f, 0.30f, 0.96f)));
	}
	for (int32 Index = 0; Index < 8; ++Index)
	{
		Phase3SecurityBoundary.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3SecurityBoundary%d"), Index),
			49,
			FLinearColor(0.20f, 0.58f, 0.72f, 0.80f)));
	}
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Phase3BaggageExceptionRoute.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3BaggageExceptionRoute%d"), Index),
			55,
			FLinearColor(0.76f, 0.42f, 0.18f, 0.62f)));
	}
	Phase3BaggageExceptionZone = CreateSpriteComponent(
		TEXT("Phase3BaggageExceptionZone"),
		54);
	Phase3BaggageExceptionStation = CreateSpriteComponent(
		TEXT("Phase3BaggageExceptionStation"),
		58);
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
	for (int32 Index = 0; Index < 23; ++Index)
	{
		Phase3Props.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase3Prop%d"), Index),
			43 + Index % 4));
	}
	InitializeTerminalLayoutPresentation();
	MatureRunway = CreateProceduralSurfaceComponent(
		TEXT("MatureRunway"),
		AMSim::WorldPresentationLayers::Runway.SortPriority);
	MatureTaxiway = CreateProceduralSurfaceComponent(
		TEXT("MatureTaxiway"),
		AMSim::WorldPresentationLayers::Taxiway.SortPriority);
	MatureApron = CreateProceduralSurfaceComponent(
		TEXT("MatureApron"),
		AMSim::WorldPresentationLayers::GateA.SortPriority);
	MatureAccessRoad = CreateProceduralSurfaceComponent(
		TEXT("MatureAccessRoad"),
		AMSim::WorldPresentationLayers::ServiceRoad.SortPriority);
	for (int32 Index = 0; Index < 3; ++Index)
	{
		MatureGatePads.Add(CreateProceduralSurfaceComponent(
			*FString::Printf(TEXT("MatureGatePad%d"), Index),
			AMSim::WorldPresentationLayers::GateA.SortPriority + Index));
	}
	for (int32 Index = 0;
		Index < static_cast<int32>(ESiteSprite::Count);
		++Index)
	{
		MatureSite.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("MatureSite%d"), Index),
			Index < 4 ? 8 + Index : 38 + (Index % 8)));
	}
	for (int32 Index = 0; Index < 6; ++Index)
	{
		MatureTaxiConnectors.Add(CreateProceduralSurfaceComponent(
			*FString::Printf(TEXT("MatureTaxiConnector%d"), Index),
			12 + Index));
	}
	for (int32 Index = 0; Index < 7; ++Index)
	{
		MatureLandsideLinks.Add(CreateProceduralSurfaceComponent(
			*FString::Printf(TEXT("MatureLandsideLink%d"), Index),
			11 + Index));
	}
	for (int32 Index = 0; Index < 10; ++Index)
	{
		MatureLandscapeClusters.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("MatureLandscapeCluster%d"), Index),
			36 + Index % 2));
	}
	for (int32 Index = 0; Index < 3; ++Index)
	{
		MatureAircraft.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("MatureAircraft%d"), Index),
			63 + Index));
	}
	MatureSelection = CreateSpriteComponent(
		TEXT("MatureSelection"),
		75,
		FLinearColor(0.29f, 0.86f, 0.94f, 0.92f));
	for (int32 Index = 0; Index < 8; ++Index)
	{
		MatureGroundVehicles.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("MatureGroundVehicle%d"), Index),
			66 + Index % 3));
	}
	for (int32 Index = 0; Index < 18; ++Index)
	{
		MaturePeople.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("MaturePerson%d"), Index),
			70 + Index % 3));
	}
	for (int32 Index = 0; Index < 8; ++Index)
	{
		MatureBags.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("MatureBag%d"), Index),
			69));
	}
	Phase4WeatherOverlay = CreateSpriteComponent(
		TEXT("Phase4WeatherOverlay"),
		76,
		FLinearColor(0.03f, 0.14f, 0.22f, 0.52f));
	for (int32 Index = 0; Index < 3; ++Index)
	{
		Phase4WetSurfaces.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase4WetSurface%d"), Index),
			47 + Index,
			FLinearColor(0.48f, 0.74f, 0.84f, 0.36f)));
	}
	for (int32 Index = 0; Index < 54; ++Index)
	{
		Phase4RainStreaks.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase4RainStreak%d"), Index),
			77,
			FLinearColor(0.68f, 0.90f, 0.98f, 0.68f)));
	}
	Phase4IncidentRunway = CreateSpriteComponent(
		TEXT("Phase4IncidentRunway"),
		78);
	Phase4AffectedAircraft = CreateSpriteComponent(
		TEXT("Phase4AffectedAircraft"),
		85);
	for (int32 Index = 0; Index < 8; ++Index)
	{
		Phase4ClosureHatch.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase4ClosureHatch%d"), Index),
			81,
			FLinearColor(0.96f, 0.25f, 0.18f, 0.86f)));
	}
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Phase4RunwayClosure.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase4RunwayClosure%d"), Index),
			81,
			FLinearColor(0.96f, 0.25f, 0.18f, 0.90f)));
	}
	for (int32 Index = 0; Index < 7; ++Index)
	{
		Phase4EmergencyRoute.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase4EmergencyRoute%d"), Index),
			79,
			FLinearColor(0.96f, 0.25f, 0.18f, 0.90f)));
	}
	for (int32 Index = 0; Index < 6; ++Index)
	{
		Phase4EmergencyRouteSegments.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase4EmergencyRouteSegment%d"), Index),
			78,
			FLinearColor(0.96f, 0.22f, 0.16f, 0.82f)));
	}
	for (int32 Index = 0; Index < 3; ++Index)
	{
		Phase4ResponseVehicles.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase4ResponseVehicle%d"), Index),
			82 + Index,
			Index == 0
				? FLinearColor(0.95f, 0.23f, 0.18f, 1.0f)
					: FLinearColor(0.98f, 0.78f, 0.20f, 1.0f)));
	}
	Phase4ProtectionZone = CreateSpriteComponent(
		TEXT("Phase4ProtectionZone"),
		80);
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Phase5WarehouseZones.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase5WarehouseZone%d"), Index),
			42 + Index));
	}
	for (int32 Index = 0; Index < 12; ++Index)
	{
		Phase5CargoStacks.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase5CargoStack%d"), Index),
			58 + Index % 3));
	}
	for (int32 Index = 0; Index < 5; ++Index)
	{
		Phase5CargoVehicles.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase5CargoVehicle%d"), Index),
			68 + Index % 3));
	}
	for (int32 Index = 0; Index < 7; ++Index)
	{
		Phase5CargoRoutes.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase5CargoRoute%d"), Index),
			57,
			FLinearColor(0.10f, 0.78f, 0.84f, 0.76f)));
	}
	for (int32 Index = 0; Index < 18; ++Index)
	{
		Phase5EventArea.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase5EventProxy%d"), Index),
			72 + Index % 3));
	}
	for (int32 Index = 0; Index < 2; ++Index)
	{
		Phase5Freighters.Add(CreateSpriteComponent(
			*FString::Printf(TEXT("Phase5Freighter%d"), Index),
			67 + Index));
	}

	TerrainSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Surfaces/S_TemperateGrass.S_TemperateGrass"));
	RunwaySprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Surfaces/S_RunwayMarked.S_RunwayMarked"));
	TaxiSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Surfaces/S_TaxiwayMarked.S_TaxiwayMarked"));
	AccessSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Surfaces/S_TaxiWear.S_TaxiWear"));
	StandSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Surfaces/S_WarmStand.S_WarmStand"));
	WhiteSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Props/S_WhiteSquare.S_WhiteSquare"));
	HutSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Props/S_OperationsHut.S_OperationsHut"));
	WindsockSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Props/S_Windsock.S_Windsock"));
	SelectionSprite = FindSprite(TEXT("/Game/Phase1/Presentation/Sprites/Props/S_SelectionRing.S_SelectionRing"));
	LoadPhase1ConstructionPreviewAssets();

	for (int32 Index = 0; Index < 16; ++Index)
	{
		const FString Path = FString::Printf(
			TEXT("/Game/Phase1/Presentation/Sprites/Aircraft/S_Cessna152_Heading_%02d.S_Cessna152_Heading_%02d"),
			Index,
			Index);
		AircraftHeadingSprites.Add(FindSprite(*Path));
	}
	static const TCHAR* OperationsSpriteNames[] = {
		TEXT("FuelTruck"),
		TEXT("BaggageTug"),
		TEXT("BaggageCartTrain"),
		TEXT("OperationsVan"),
		TEXT("FireRescueTruck"),
		TEXT("ConstructionTruck"),
		TEXT("ShuttleBus"),
		TEXT("Taxi"),
		TEXT("RentalCar"),
		TEXT("Deicer"),
		TEXT("RampWorker"),
		TEXT("ConstructionWorker"),
		TEXT("SecurityOfficer"),
		TEXT("TerminalAgent"),
		TEXT("PassengerFamily"),
		TEXT("Passenger"),
		TEXT("Suitcase"),
		TEXT("SafetyCones"),
		TEXT("PortableStairs"),
		TEXT("PushbackTug")};
	static_assert(
		UE_ARRAY_COUNT(OperationsSpriteNames) ==
			static_cast<int32>(EOperationsSprite::Count));
	for (const TCHAR* SpriteName : OperationsSpriteNames)
	{
		const FString Path = FString::Printf(
			TEXT("/Game/Phase45/Presentation/Sprites/Operations/S_%s.S_%s"),
			SpriteName,
			SpriteName);
		OperationsSprites.Add(FindSprite(*Path));
	}
	static const TCHAR* TerminalSpriteNames[] = {
		TEXT("CheckInDesk"),
		TEXT("BagDropDesk"),
		TEXT("SecurityScanner"),
		TEXT("QueueBarriers"),
		TEXT("GatePodium"),
		TEXT("SeatingCluster"),
		TEXT("BaggageConveyor"),
		TEXT("SortingTable"),
		TEXT("BaggageCart"),
		TEXT("ReclaimCarousel"),
		TEXT("InformationDesk"),
		TEXT("RestroomBlock"),
		TEXT("EntranceDoors"),
		TEXT("SecureDoor"),
		TEXT("PartitionWall"),
		TEXT("CurbsideShelter"),
		TEXT("BusStopShelter"),
		TEXT("ParkingKiosk"),
		TEXT("TerminalFloor"),
		TEXT("SecureFloor"),
		TEXT("CautionHatch"),
		TEXT("DirectionArrow"),
		TEXT("AccessibleRoute"),
		TEXT("ProtectionZone")};
	static_assert(
		UE_ARRAY_COUNT(TerminalSpriteNames) ==
			static_cast<int32>(ETerminalSprite::Count));
	for (const TCHAR* SpriteName : TerminalSpriteNames)
	{
		const FString Path = FString::Printf(
			TEXT("/Game/Phase45/Presentation/Sprites/Terminal/S_%s.S_%s"),
			SpriteName,
			SpriteName);
		TerminalSprites.Add(FindSprite(*Path));
	}
	static const TCHAR* SiteSpriteNames[] = {
		TEXT("RunwayAsphalt"),
		TEXT("TaxiwayAsphalt"),
		TEXT("ApronStand"),
		TEXT("AccessRoad"),
		TEXT("RegionalTerminal"),
		TEXT("GAHangars"),
		TEXT("OperationsStation"),
		TEXT("FuelFarm"),
		TEXT("ParkingLot"),
		TEXT("BusTaxiBay"),
		TEXT("RailPlatform"),
		TEXT("DropoffIsland"),
		TEXT("TreeCluster"),
		TEXT("LandscapeCluster"),
		TEXT("PerimeterGate"),
		TEXT("ApronFixtures")};
	static_assert(
		UE_ARRAY_COUNT(SiteSpriteNames) ==
			static_cast<int32>(ESiteSprite::Count));
	for (const TCHAR* SpriteName : SiteSpriteNames)
	{
		const FString Path = FString::Printf(
			TEXT("/Game/Phase45/Presentation/Sprites/Site/S_%s.S_%s"),
			SpriteName,
			SpriteName);
		SiteSprites.Add(FindSprite(*Path));
	}
	Phase5FreighterSprites.Add(FindSprite(
		TEXT("/Game/Phase5/Presentation/Sprites/Aircraft/S_Riverlark_F28.S_Riverlark_F28")));
	Phase5FreighterSprites.Add(FindSprite(
		TEXT("/Game/Phase5/Presentation/Sprites/Aircraft/S_Hearthwing_F62.S_Hearthwing_F62")));
	const auto OperationSprite =
		[this](const EOperationsSprite Sprite)
		{
			return OperationsSprites[
				static_cast<int32>(Sprite)].Get();
		};
	const auto TerminalSprite =
		[this](const ETerminalSprite Sprite)
		{
			return TerminalSprites[
				static_cast<int32>(Sprite)].Get();
		};
	const auto SiteSprite =
		[this](const ESiteSprite Sprite)
		{
			return SiteSprites[
				static_cast<int32>(Sprite)].Get();
		};
	FinalizePhase1OperationsPresentation(
		OperationSprite(EOperationsSprite::ConstructionTruck),
		OperationSprite(EOperationsSprite::ConstructionWorker),
		OperationSprite(EOperationsSprite::SafetyCones),
		TerminalSprite(ETerminalSprite::ProtectionZone),
		OperationSprite(EOperationsSprite::FuelTruck),
		OperationSprite(EOperationsSprite::RampWorker),
		TerminalSprite(ETerminalSprite::DirectionArrow));

	ConfigureSprite(
		Terrain,
		TerrainSprite,
		FVector(0.0, 0.0, AMSim::WorldPresentationLayers::Terrain.Height),
		FVector(100.0, 1.0, 100.0));
	Terrain->SetSpriteColor(FLinearColor(0.25f, 0.34f, 0.22f, 1.0f));
	ConfigureSprite(
		OperationsHut,
		SiteSprite(ESiteSprite::RegionalTerminal),
		FVector(-27000.0, 36000.0,
			AMSim::WorldPresentationLayers::Structure.Height),
		FVector(11.0, 1.0, 11.0));
	OperationsHut->SetRelativeRotation(
		FRotator(0.0f, 0.0f, SpritePlaneRoll));
	ConfigureSprite(Windsock, WindsockSprite, FVector(14000.0, -45000.0, 45.0), FVector(3.0, 1.0, 3.0));
	Aircraft->SetRelativeScale3D(FVector(10.0, 1.0, 10.0));
	ConfigureSprite(Selection, SelectionSprite, FVector(-32000.0, 23000.0, 70.0), FVector(2.2, 1.0, 2.2));
	FinalizePhase1ConstructionPreviewPresentation();
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
			OperationSprite(
				Index == 0
					? EOperationsSprite::FuelTruck
					: Index == 1
						? EOperationsSprite::BaggageTug
						: Index == 2
							? EOperationsSprite::OperationsVan
							: EOperationsSprite::PushbackTug),
			FVector(-39000.0, 20000.0 + Index * 6000.0, 64.0 + Index),
			FVector(5.0, 1.0, 5.0));
		Phase2Vehicles[Index]->SetSpriteColor(FLinearColor::White);
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
			TerminalSprite(
				Index == 1
					? ETerminalSprite::SecureFloor
					: ETerminalSprite::TerminalFloor),
			FloorLocations[Index],
			FloorScales[Index] * 0.055);
		Phase3Floor[Index]->SetSpriteColor(FLinearColor::White);
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
			TerminalSprite(
				Index == 2 || Index == 3 || Index == 4
					? ETerminalSprite::SecureFloor
					: ETerminalSprite::TerminalFloor),
			RoomLocations[Index],
			RoomScales[Index]);
		const FLinearColor RoomPalette[] = {
			FLinearColor(0.88f, 0.92f, 0.82f, 1.0f),
			FLinearColor(0.94f, 0.87f, 0.72f, 1.0f),
			FLinearColor(0.65f, 0.88f, 0.91f, 1.0f),
			FLinearColor(0.70f, 0.91f, 0.93f, 1.0f),
			FLinearColor(0.66f, 0.85f, 0.90f, 1.0f),
			FLinearColor(0.84f, 0.72f, 0.90f, 1.0f),
			FLinearColor(0.95f, 0.82f, 0.61f, 1.0f),
			FLinearColor(0.86f, 0.74f, 0.91f, 1.0f),
			FLinearColor(0.79f, 0.90f, 0.80f, 1.0f),
			FLinearColor(0.72f, 0.78f, 0.76f, 1.0f)};
		Phase3Rooms[Index]->SetSpriteColor(RoomPalette[Index]);
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
			OperationSprite(
				Index % 11 == 0
					? EOperationsSprite::PassengerFamily
					: EOperationsSprite::Passenger),
			FVector::ZeroVector,
			FVector(2.7, 1.0, 2.7));
		Phase3Passengers[Index]->SetSpriteColor(FLinearColor::White);
	}
	for (int32 Index = 0; Index < Phase3Bags.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3Bags[Index],
			OperationSprite(EOperationsSprite::Suitcase),
			FVector::ZeroVector,
			FVector(1.35, 1.0, 1.35));
		Phase3Bags[Index]->SetSpriteColor(FLinearColor::White);
	}
	const EOperationsSprite Phase3VehicleSprites[] = {
		EOperationsSprite::Taxi,
		EOperationsSprite::ShuttleBus,
		EOperationsSprite::RentalCar,
		EOperationsSprite::BaggageTug,
		EOperationsSprite::BaggageCartTrain,
		EOperationsSprite::OperationsVan};
	const FVector Phase3VehicleLocations[] = {
		FVector(-18000.0, -23000.0, 58.0),
		FVector(-18000.0, -6000.0, 59.0),
		FVector(-23000.0, -18000.0, 60.0),
		FVector(5000.0, -5500.0, 61.0),
		FVector(5000.0, 6500.0, 62.0),
		FVector(8000.0, 25000.0, 63.0)};
	for (int32 Index = 0; Index < Phase3Vehicles.Num(); ++Index)
	{
		ConfigureSprite(
			Phase3Vehicles[Index],
			OperationSprite(Phase3VehicleSprites[Index]),
			Phase3VehicleLocations[Index],
			FVector(6.5, 1.0, 6.5));
		Phase3Vehicles[Index]->SetSpriteColor(FLinearColor::White);
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
			OperationSprite(
				Index < 4
					? EOperationsSprite::SecurityOfficer
					: Index < 8
						? EOperationsSprite::TerminalAgent
						: EOperationsSprite::RampWorker),
			WorkArea + FVector(
				(Index % 4) * 900.0,
				(Index % 2) * 900.0,
				0.0),
			FVector(2.8, 1.0, 2.8));
		Phase3Staff[Index]->SetSpriteColor(FLinearColor::White);
	}
	const FVector PropLocations[] = {
		FVector(-6000.0, -24000.0, 43.0),
		FVector(3000.0, -24000.0, 44.0),
		FVector(0.0, -3000.0, 45.0),
		FVector(-5000.0, -3000.0, 44.0),
		FVector(23000.0, -16000.0, 45.0),
		FVector(17000.0, -8000.0, 44.0),
		FVector(3000.0, 7000.0, 43.0),
		FVector(5000.0, 15500.0, 44.0),
		FVector(12000.0, 15500.0, 45.0),
		FVector(-16000.0, -10000.0, 44.0),
		FVector(-22000.0, -22000.0, 45.0),
		FVector(10000.0, -5000.0, 43.0),
		FVector(-27000.0, -25000.0, 44.0),
		FVector(6000.0, 4500.0, 45.0),
		FVector(-8000.0, 6000.0, 43.0),
		FVector(-35000.0, -10000.0, 44.0),
		FVector(-35000.0, 9000.0, 45.0),
		FVector(-35000.0, -27000.0, 43.0),
		FVector(-5000.0, -12000.0, 44.0),
		FVector(0.0, -3000.0, 43.0),
		FVector(27000.0, -21000.0, 45.0),
		FVector(-10000.0, -20000.0, 45.0),
		FVector(2000.0, -5000.0, 46.0)};
	for (int32 Index = 0; Index < Phase3Props.Num(); ++Index)
	{
		const bool bFloorMarking =
			Index >= static_cast<int32>(
				ETerminalSprite::TerminalFloor);
		ConfigureSprite(
			Phase3Props[Index],
			TerminalSprite(static_cast<ETerminalSprite>(Index)),
			PropLocations[Index],
			bFloorMarking
				? FVector(7.0, 1.0, 7.0)
				: FVector(7.5, 1.0, 7.5));
		Phase3Props[Index]->SetSpriteColor(FLinearColor::White);
	}
	FinalizeTerminalPresentation(
		TerminalSprite(ETerminalSprite::DirectionArrow),
		WhiteSprite,
		TerminalSprite(ETerminalSprite::SecureDoor),
		TerminalSprite(ETerminalSprite::PartitionWall),
		TerminalSprite(ETerminalSprite::CautionHatch),
		TerminalSprite(ETerminalSprite::SortingTable));
	const FVector MatureSiteLocations[] = {
		FVector(32000.0, 0.0, 8.0),
		FVector(21000.0, 0.0, 9.0),
		FVector(7000.0, 0.0, 10.0),
		FVector(-34000.0, 0.0, 11.0),
		FVector(-7500.0, 0.0, 40.0),
		FVector(7000.0, -31000.0, 41.0),
		FVector(7000.0, 30000.0, 42.0),
		FVector(1000.0, 42000.0, 43.0),
		FVector(-23500.0, -25000.0, 44.0),
		FVector(-22500.0, -4000.0, 45.0),
		FVector(-24000.0, 27000.0, 44.0),
		FVector(-13500.0, 8000.0, 43.0),
		FVector(40500.0, -41000.0, 39.0),
		FVector(40500.0, 41000.0, 39.0),
		FVector(-35000.0, -41000.0, 42.0),
		FVector(7000.0, 21000.0, 46.0)};
	const FVector MatureSiteScales[] = {
		FVector(82.0, 1.0, 13.0),
		FVector(76.0, 1.0, 10.0),
		FVector(34.0, 1.0, 30.0),
		FVector(78.0, 1.0, 9.0),
		FVector(25.0, 1.0, 25.0),
		FVector(22.0, 1.0, 22.0),
		FVector(15.0, 1.0, 15.0),
		FVector(14.0, 1.0, 14.0),
		FVector(20.0, 1.0, 20.0),
		FVector(18.0, 1.0, 18.0),
		FVector(20.0, 1.0, 20.0),
		FVector(18.0, 1.0, 18.0),
		FVector(14.0, 1.0, 14.0),
		FVector(12.0, 1.0, 12.0),
		FVector(13.0, 1.0, 13.0),
		FVector(13.0, 1.0, 13.0)};
	static_assert(
		UE_ARRAY_COUNT(MatureSiteLocations) ==
			static_cast<int32>(ESiteSprite::Count));
	static_assert(
		UE_ARRAY_COUNT(MatureSiteScales) ==
			static_cast<int32>(ESiteSprite::Count));
	for (int32 Index = 0; Index < MatureSite.Num(); ++Index)
	{
		ConfigureSprite(
			MatureSite[Index],
			SiteSprite(static_cast<ESiteSprite>(Index)),
			MatureSiteLocations[Index],
			MatureSiteScales[Index]);
		MatureSite[Index]->SetSpriteColor(FLinearColor::White);
		MatureSite[Index]->SetVisibility(false);
	}
	InitializeMatureInfrastructurePresentation();
	FinalizeIncidentPresentation(
		SiteSprite(ESiteSprite::RunwayAsphalt),
		SiteSprite(ESiteSprite::TaxiwayAsphalt),
		SiteSprite(ESiteSprite::ApronStand));
	FinalizePhase5Presentation(
		Phase5FreighterSprites[0],
		Phase5FreighterSprites[1]);
	InitializePhase6Presentation();
	const FVector LandscapeLocations[] = {
		FVector(39000.0, -26000.0, 36.0),
		FVector(39000.0, -8000.0, 36.0),
		FVector(39000.0, 12000.0, 36.0),
		FVector(39000.0, 30000.0, 36.0),
		FVector(23000.0, -43000.0, 36.0),
		FVector(22000.0, 43000.0, 36.0),
		FVector(-5000.0, -43000.0, 36.0),
		FVector(-6000.0, 43000.0, 36.0),
		FVector(-36000.0, -25000.0, 36.0),
		FVector(-36000.0, 30000.0, 36.0)};
	for (int32 Index = 0; Index < MatureLandscapeClusters.Num(); ++Index)
	{
		ConfigureSprite(
			MatureLandscapeClusters[Index],
			SiteSprite(
				Index % 3 == 0
					? ESiteSprite::LandscapeCluster
					: ESiteSprite::TreeCluster),
			LandscapeLocations[Index],
			FVector(
				7.0f + static_cast<float>(Index % 3) * 1.5f,
				1.0f,
				7.0f + static_cast<float>(Index % 3) * 1.5f));
		MatureLandscapeClusters[Index]->SetVisibility(false);
	}
	const FVector MatureAircraftLocations[] = {
		FVector(7000.0, -9000.0, 63.0),
		FVector(7000.0, 9000.0, 64.0),
		FVector(23000.0, -28000.0, 65.0)};
	const float MatureAircraftScales[] = {12.0f, 11.0f, 13.0f};
	for (int32 Index = 0; Index < MatureAircraft.Num(); ++Index)
	{
		const int32 HeadingIndex = Index * 4;
		ConfigureSprite(
			MatureAircraft[Index],
			AircraftHeadingSprites.IsValidIndex(HeadingIndex)
				? AircraftHeadingSprites[HeadingIndex]
				: WhiteSprite,
			MatureAircraftLocations[Index],
			FVector(
				MatureAircraftScales[Index],
				1.0,
				MatureAircraftScales[Index]));
		MatureAircraft[Index]->SetVisibility(false);
	}
	ConfigureSprite(
		MatureSelection,
		SelectionSprite,
		FVector(7000.0, -9000.0, 75.0),
		FVector(16.0f, 1.0f, 16.0f));
	MatureSelection->SetVisibility(false);
	const EOperationsSprite MatureVehicleSprites[] = {
		EOperationsSprite::BaggageTug,
		EOperationsSprite::BaggageCartTrain,
		EOperationsSprite::FuelTruck,
		EOperationsSprite::OperationsVan,
		EOperationsSprite::ShuttleBus,
		EOperationsSprite::Taxi,
		EOperationsSprite::RentalCar,
		EOperationsSprite::FireRescueTruck};
	const FVector MatureVehicleLocations[] = {
		FVector(8500.0, -10000.0, 66.0),
		FVector(8500.0, 10000.0, 67.0),
		FVector(7000.0, 24000.0, 68.0),
		FVector(6000.0, 33000.0, 69.0),
		FVector(-23000.0, -5000.0, 66.0),
		FVector(-25500.0, -21000.0, 67.0),
		FVector(-26000.0, 10000.0, 68.0),
		FVector(10000.0, 30000.0, 69.0)};
	for (int32 Index = 0; Index < MatureGroundVehicles.Num(); ++Index)
	{
		ConfigureSprite(
			MatureGroundVehicles[Index],
			OperationSprite(MatureVehicleSprites[Index]),
			MatureVehicleLocations[Index],
			FVector(5.5f, 1.0f, 5.5f));
		MatureGroundVehicles[Index]->SetVisibility(false);
	}
	const FVector MaturePeopleAreas[] = {
		FVector(-10500.0, -12000.0, 70.0),
		FVector(-10500.0, 0.0, 70.0),
		FVector(-10500.0, 12000.0, 70.0),
		FVector(7000.0, -12000.0, 70.0),
		FVector(7000.0, 12000.0, 70.0),
		FVector(-22000.0, -5000.0, 70.0)};
	for (int32 Index = 0; Index < MaturePeople.Num(); ++Index)
	{
		const FVector Base = MaturePeopleAreas[Index % UE_ARRAY_COUNT(MaturePeopleAreas)];
		const FVector Offset(
			(Index / UE_ARRAY_COUNT(MaturePeopleAreas)) * 950.0,
			(Index % 3) * 800.0,
			static_cast<double>(Index % 3));
		ConfigureSprite(
			MaturePeople[Index],
			OperationSprite(
				Index % 7 == 0
					? EOperationsSprite::PassengerFamily
					: Index % 5 == 0
						? EOperationsSprite::RampWorker
						: EOperationsSprite::Passenger),
			Base + Offset,
			FVector(1.8f, 1.0f, 1.8f));
		MaturePeople[Index]->SetVisibility(false);
	}
	for (int32 Index = 0; Index < MatureBags.Num(); ++Index)
	{
		ConfigureSprite(
			MatureBags[Index],
			OperationSprite(EOperationsSprite::Suitcase),
			FVector(
				6500.0 + (Index % 4) * 750.0,
				-5000.0 + (Index / 4) * 10000.0,
				69.0 + Index % 2),
			FVector(1.1f, 1.0f, 1.1f));
		MatureBags[Index]->SetVisibility(false);
	}
	ConfigureSprite(
		Phase3Aircraft,
		AircraftHeadingSprites.IsValidIndex(0)
			? AircraftHeadingSprites[0]
			: WhiteSprite,
		FVector(33000.0, -28600.0, 67.0),
		FVector(15.0, 1.0, 15.0));
	ConfigureSprite(
		Phase4WeatherOverlay,
		WhiteSprite,
		FVector(0.0, 0.0, 76.0),
		FVector(900.0, 1.0, 650.0));
	for (int32 Index = 0; Index < Phase4RainStreaks.Num(); ++Index)
	{
		ConfigureSprite(
			Phase4RainStreaks[Index],
			WhiteSprite,
			FVector(
				-47000.0 + (Index % 12) * 8500.0,
				-40000.0 + (Index / 12) * 20000.0,
				77.0),
			FVector(
				18.0f + static_cast<float>(Index % 5) * 2.6f,
				1.0f,
				0.8f + static_cast<float>(Index % 3) * 0.16f));
		Phase4RainStreaks[Index]->SetRelativeRotation(
			FRotator(0.0f, -12.0f, SpritePlaneRoll));
		Phase4RainStreaks[Index]->SetVisibility(false);
	}
	ConfigureSprite(
		Phase4IncidentRunway,
		RunwaySprite,
		FVector(27000.0, 0.0, 78.0),
		FVector(2.8, 1.0, 45.0));
	Phase4IncidentRunway->SetVisibility(false);
	ConfigureSprite(
		Phase4AffectedAircraft,
		AircraftHeadingSprites.IsValidIndex(4)
			? AircraftHeadingSprites[4]
			: WhiteSprite,
		FVector(24000.0, -21000.0, 85.0),
		FVector(14.0f, 1.0f, 14.0f));
	Phase4AffectedAircraft->SetSpriteColor(
		FLinearColor(0.92f, 0.93f, 0.96f, 1.0f));
	Phase4AffectedAircraft->SetVisibility(false);
	for (int32 Index = 0; Index < Phase4ClosureHatch.Num(); ++Index)
	{
		ConfigureSprite(
			Phase4ClosureHatch[Index],
			TerminalSprite(ETerminalSprite::ProtectionZone),
			FVector(
				20500.0 + (Index % 2) * 5500.0,
				-28500.0 + (Index / 2) * 5000.0,
				81.0 + Index % 2),
			FVector(6.2f, 1.0f, 6.2f));
		Phase4ClosureHatch[Index]->SetVisibility(false);
	}
	const FVector ClosureLocations[] = {
		FVector(24000.0, -30000.0, 82.0),
		FVector(24000.0, -12000.0, 82.0),
		FVector(17000.0, -21000.0, 82.0),
		FVector(31000.0, -21000.0, 82.0)};
	for (int32 Index = 0; Index < Phase4RunwayClosure.Num(); ++Index)
	{
		ConfigureSprite(
			Phase4RunwayClosure[Index],
			OperationSprite(EOperationsSprite::SafetyCones),
			ClosureLocations[Index],
			FVector(5.8, 1.0, 5.8));
		Phase4RunwayClosure[Index]->SetSpriteColor(
			FLinearColor::White);
		Phase4RunwayClosure[Index]->SetVisibility(false);
	}
	const FVector RouteLocations[] = {
		FVector(-15000.0, 28000.0, 79.0),
		FVector(-8000.0, 20000.0, 79.0),
		FVector(-2000.0, 12000.0, 79.0),
		FVector(4000.0, 4000.0, 79.0),
		FVector(10000.0, -4000.0, 79.0),
		FVector(16000.0, -12000.0, 79.0),
		FVector(22000.0, -18000.0, 79.0)};
	for (int32 Index = 0; Index < Phase4EmergencyRoute.Num(); ++Index)
	{
		ConfigureSprite(
			Phase4EmergencyRoute[Index],
			TerminalSprite(ETerminalSprite::DirectionArrow),
			RouteLocations[Index],
			FVector(8.0f, 1.0f, 8.0f));
		Phase4EmergencyRoute[Index]->SetVisibility(false);
	}
	for (int32 Index = 0;
		Index < Phase4EmergencyRouteSegments.Num();
		++Index)
	{
		const FVector Start = RouteLocations[Index];
		const FVector End = RouteLocations[Index + 1];
		const FVector Delta = End - Start;
		const float RouteLength = FVector2D(Delta.X, Delta.Y).Size();
		ConfigureSprite(
			Phase4EmergencyRouteSegments[Index],
			WhiteSprite,
			(Start + End) * 0.5,
			FVector(RouteLength / 200.0f, 1.0f, 2.2f));
		const float RouteAngle = FMath::RadiansToDegrees(
			FMath::Atan2(Delta.Y, Delta.X));
		Phase4EmergencyRouteSegments[Index]->SetRelativeRotation(
			FRotator(0.0f, RouteAngle, SpritePlaneRoll));
		Phase4EmergencyRouteSegments[Index]->SetVisibility(false);
	}
	for (int32 Index = 0; Index < Phase4ResponseVehicles.Num(); ++Index)
	{
		const EOperationsSprite ResponseSprite =
			Index == 0
				? EOperationsSprite::PushbackTug
				: Index == 1
					? EOperationsSprite::FireRescueTruck
					: EOperationsSprite::OperationsVan;
		ConfigureSprite(
			Phase4ResponseVehicles[Index],
			OperationSprite(ResponseSprite),
			FVector(
				-8000.0 + Index * 12000.0,
				18000.0 - Index * 14000.0,
				82.0 + Index),
			FVector(9.5f, 1.0f, 9.5f));
		Phase4ResponseVehicles[Index]->SetSpriteColor(
			FLinearColor::White);
		Phase4ResponseVehicles[Index]->SetVisibility(false);
	}
	ConfigureSprite(
		Phase4ProtectionZone,
		TerminalSprite(ETerminalSprite::ProtectionZone),
		FVector(24000.0, -21000.0, 80.0),
		FVector(10.0f, 1.0f, 10.0f));
	Phase4ProtectionZone->SetVisibility(false);
	Phase4WeatherOverlay->SetVisibility(false);
	ConfigureSprite(
		ExpansionOverlay,
		StandSprite,
		FVector(-35000.0, 38000.0, 35.0),
		FVector(10.0, 1.0, 8.0));
	ConfigureSprite(
		ExpansionClosureOverlay,
		TerminalSprite(ETerminalSprite::ProtectionZone),
		FVector(-24000.0, 18000.0, 38.0),
		FVector(9.0, 1.0, 2.2));
	ConfigureSprite(
		IncidentOverlay,
		SelectionSprite,
		FVector(-32000.0, 23000.0, 80.0),
		FVector(3.2, 1.0, 3.2));
	ExpansionOverlay->SetVisibility(false);
	ExpansionClosureOverlay->SetVisibility(false);
	IncidentOverlay->SetVisibility(false);

	ApplyPhase1Geometry(AMSim::CreateDefaultStarterPlan());
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

UAMSimProceduralSurfaceComponent*
AAMSimWorldPresenter::CreateProceduralSurfaceComponent(
	const TCHAR* Name,
	const int32 SortPriority)
{
	UAMSimProceduralSurfaceComponent* Component =
		CreateDefaultSubobject<UAMSimProceduralSurfaceComponent>(Name);
	Component->SetupAttachment(Root);
	Component->SetPresentationLayer(0.0, SortPriority);
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
	return TerrainSprite && RunwaySprite && TaxiSprite && AccessSprite &&
		StandSprite && WhiteSprite &&
		HutSprite && WindsockSprite && SelectionSprite &&
		ConstructionSnapSprite && ConstructionConnectedSprite &&
		ConstructionCrossingSprite && ConstructionInvalidSprite &&
		ConstructionPreviewMaterial &&
		AircraftHeadingSprites.Num() == 16 &&
		Algo::AllOf(
			AircraftHeadingSprites,
			[](const UPaperSprite* Sprite)
				{
					return Sprite != nullptr;
				}) &&
		OperationsSprites.Num() ==
			static_cast<int32>(EOperationsSprite::Count) &&
		Algo::AllOf(
			OperationsSprites,
			[](const UPaperSprite* Sprite)
				{
					return Sprite != nullptr;
				}) &&
		TerminalSprites.Num() ==
			static_cast<int32>(ETerminalSprite::Count) &&
		Algo::AllOf(
			TerminalSprites,
			[](const UPaperSprite* Sprite)
				{
					return Sprite != nullptr;
				}) &&
		SiteSprites.Num() ==
			static_cast<int32>(ESiteSprite::Count) &&
		Algo::AllOf(
			SiteSprites,
			[](const UPaperSprite* Sprite)
				{
					return Sprite != nullptr;
				});
}

bool AAMSimWorldPresenter::HasDistinctStarterFacilityAssets() const
{
	const UPaperSprite* TerminalFacilitySprite =
		OperationsHut ? OperationsHut->GetSprite() : nullptr;
	return Stand && GateB &&
		Stand->GetSurfaceVertexCountForTest() > 0 &&
		GateB->GetSurfaceVertexCountForTest() > 0 &&
		TerminalFacilitySprite &&
		TerminalFacilitySprite != HutSprite;
}

bool AAMSimWorldPresenter::HasDistinctPhase1MovementSurfaceAssets() const
{
	return Runway && Taxiway && Access &&
		Runway->GetSurfaceVertexCountForTest() >
			Taxiway->GetSurfaceVertexCountForTest() &&
		Taxiway->GetSurfaceVertexCountForTest() !=
			Access->GetSurfaceVertexCountForTest() &&
		Runway->HasCookerVisibleMaterialForTest() &&
		Taxiway->HasCookerVisibleMaterialForTest() &&
		Access->HasCookerVisibleMaterialForTest();
}

void AAMSimWorldPresenter::ApplySnapshot(
	const AMSim::FPhase1QuerySnapshot& Query,
	const AMSim::FPhase1State& State)
{
	if (LastAppliedRevision == Query.Revision)
	{
		return;
	}
	LastAppliedRevision = Query.Revision;
	bPhase1AirportInitialized = State.bInitialized;
	if (State.Project.Stage != AMSim::EConstructionStage::None)
	{
		ApplyPhase1Geometry(State.Project.Proposal);
	}
	else
	{
		ApplyPhase1Geometry(AMSim::CreateDefaultStarterPlan());
	}
	const bool bFacilitiesVisible = Query.ConstructionStage != AMSim::EConstructionStage::None;
	const bool bOperational = Query.ConstructionStage >= AMSim::EConstructionStage::ReadyToOpen;
	SetFacilitiesVisible(bFacilitiesVisible, bOperational, State.bInitialized);
	SetAircraftState(Query, State);
	RefreshPhase1OperationsPresentation(Query, State);
	RefreshPhase1OverlayPresentation();
	RefreshTerminalLayoutPresentation(
		CachedTerminalLayoutSnapshot,
		CachedTerminalPhase3State,
		true);
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
	ExpansionClosureOverlay->SetVisibility(
		Query.bInitialized &&
		State.Expansion.Stage != AMSim::EExpansionStage::None &&
		State.Expansion.Stage != AMSim::EExpansionStage::Operational);
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
	CachedTerminalLayoutSnapshot = Query.TerminalLayout;
	CachedTerminalPhase3State = State;
	RefreshTerminalLayoutPresentation(Query.TerminalLayout, State);
	const bool bShowWorld =
		Query.bInitialized &&
		Query.TerminalStage >= AMSim::ETerminalConstructionStage::ShellReady;
	bPhase3RoutesConnected =
		Query.RequiredConnectionCount > 0 &&
		Query.ConnectedCount == Query.RequiredConnectionCount;
	bPhase3BaggageExceptionActive =
		State.Bags.ContainsByPredicate(
			[](const AMSim::FBagRecord& Bag)
			{
				return Bag.JourneyState ==
					AMSim::EBagJourneyState::Exception;
			});
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
	ExpansionClosureOverlay->SetVisibility(false);
	IncidentOverlay->SetVisibility(false);
	Terrain->SetSpriteColor(FLinearColor(0.20f, 0.30f, 0.23f, 1.0f));
	const FLinearColor SecurityBoundaryTint =
		Query.bSecurityIntegrityValid
			? FLinearColor::White
			: FLinearColor(1.0f, 0.42f, 0.36f, 1.0f);
	for (UPaperSpriteComponent* Component : Phase3SecurityBoundary)
	{
		Component->SetSpriteColor(SecurityBoundaryTint);
	}
	const FLinearColor ExceptionRouteTint =
		bPhase3BaggageExceptionActive
			? FLinearColor(1.0f, 0.32f, 0.25f, 1.0f)
			: FLinearColor(0.76f, 0.42f, 0.18f, 0.62f);
	for (UPaperSpriteComponent* Component : Phase3BaggageExceptionRoute)
	{
		Component->SetSpriteColor(ExceptionRouteTint);
	}
	Phase3BaggageExceptionZone->SetSpriteColor(
		bPhase3BaggageExceptionActive
			? FLinearColor(1.0f, 0.30f, 0.22f, 0.88f)
			: FLinearColor(0.72f, 0.46f, 0.24f, 0.52f));

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
		Base.Y *= 1.30;
		UPaperSpriteComponent* Proxy = Phase3Passengers[PassengerProxyIndex];
		Proxy->SetRelativeLocation(Base);
		Proxy->SetSpriteColor(
			Passenger.bRequiresAccessibleRoute
				? FLinearColor(1.0f, 0.92f, 0.58f, 1.0f)
				: FLinearColor::White);
		Proxy->SetVisibility(!bMatureOverviewMode);
		++PassengerProxyIndex;
	}
	Phase3PassengerAvailableCount = PassengerProxyIndex;
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
		case AMSim::EBagJourneyState::Exception:
			Base = FVector(20500.0, 16500.0, 65.0);
			break;
		default:
			Base = FVector(5000.0, 18000.0, 65.0);
			break;
		}
		Base.X += (BagProxyIndex % 9) * 600.0;
		Base.Y += ((BagProxyIndex / 9) % 4) * 560.0;
		Base.Y *= 1.30;
		UPaperSpriteComponent* Proxy = Phase3Bags[BagProxyIndex];
		Proxy->SetRelativeLocation(Base);
		Proxy->SetSpriteColor(
			Bag.JourneyState == AMSim::EBagJourneyState::Exception
				? FLinearColor(1.0f, 0.48f, 0.42f, 1.0f)
				: FLinearColor::White);
		Proxy->SetVisibility(!bMatureOverviewMode);
		++BagProxyIndex;
	}
	Phase3BagAvailableCount = BagProxyIndex;
	for (; BagProxyIndex < Phase3Bags.Num(); ++BagProxyIndex)
	{
		Phase3Bags[BagProxyIndex]->SetVisibility(false);
	}
	bPhase3AircraftAvailable =
		State.Flight.Id.IsValid() &&
		State.Flight.State != AMSim::EPhase3FlightState::Completed;
	Phase3Aircraft->SetVisibility(
		!bMatureOverviewMode && bPhase3AircraftAvailable);
	RefreshPhase3OverlayVisibility();
	RefreshIncidentPresentationVisibility();
	RefreshTerminalLayoutPresentation(Query.TerminalLayout, State, true);
}

void AAMSimWorldPresenter::ApplyPhase4Snapshot(
	const AMSim::FPhase4QuerySnapshot& Query,
	const AMSim::FPhase4State& State)
{
	const bool bIncidentActive =
		Query.bInitialized &&
		Query.IncidentLifecycle >= AMSim::EPhase4IncidentLifecycle::Alerted &&
		Query.IncidentLifecycle < AMSim::EPhase4IncidentLifecycle::Recovered;
	bIncidentPresentationMode = bIncidentActive;
	bPhase4IncidentWorldVisible = bIncidentActive;
	bPhase4RunwayClosed =
		bIncidentActive && State.Incident.bRunwayClosed;
	bPhase4RouteVisible =
		bIncidentActive && State.Incident.bTowDispatched;
	bPhase4ProtectionVisible =
		bIncidentActive && State.Incident.bAreaProtected;
	if (bIncidentActive)
	{
		SetMatureOverviewMode(true);
	}
	if (LastAppliedPhase4Revision == Query.Revision)
	{
		RefreshIncidentPresentationVisibility();
		return;
	}
	LastAppliedPhase4Revision = Query.Revision;
	MatureAircraftAvailableCount = FMath::Clamp(
		Query.FlightCount - Query.CompletedFlightCount,
		0,
		MatureAircraft.Num());
	for (int32 Index = 0; Index < MatureAircraft.Num(); ++Index)
	{
		MatureAircraft[Index]->SetVisibility(
			bPhase3WorldVisible &&
			bMatureOverviewMode &&
			Index < MatureAircraftAvailableCount);
	}
	MatureGroundVehicleAvailableCount =
		Query.bInitialized
			? FMath::Clamp(
				4 + Query.AcceptedContractCount,
				0,
				MatureGroundVehicles.Num())
			: 0;
	MaturePeopleAvailableCount =
		Query.bInitialized
			? FMath::Clamp(
				(State.BorderProcesses.Num() + State.Connections.Num() + 11) /
					12,
				0,
				MaturePeople.Num())
			: 0;
	MatureBagAvailableCount =
		Query.bInitialized
			? FMath::Clamp(
				(State.TransferBags.Num() + 3) / 4,
				0,
				MatureBags.Num())
			: 0;
	for (int32 Index = 0; Index < MatureGroundVehicles.Num(); ++Index)
	{
		MatureGroundVehicles[Index]->SetVisibility(
			bPhase3WorldVisible &&
			bMatureOverviewMode &&
			Index < MatureGroundVehicleAvailableCount);
	}
	for (int32 Index = 0; Index < MaturePeople.Num(); ++Index)
	{
		MaturePeople[Index]->SetVisibility(
			bPhase3WorldVisible &&
			bMatureOverviewMode &&
			Index < MaturePeopleAvailableCount);
	}
	for (int32 Index = 0; Index < MatureBags.Num(); ++Index)
	{
		MatureBags[Index]->SetVisibility(
			bPhase3WorldVisible &&
			bMatureOverviewMode &&
			Index < MatureBagAvailableCount);
	}
	Phase4WeatherOverlay->SetVisibility(false);
	for (UPaperSpriteComponent* Component : Phase4RainStreaks)
	{
		Component->SetVisibility(bIncidentActive);
	}
	Phase4IncidentRunway->SetVisibility(
		bIncidentActive && !bMatureOverviewMode);
	for (int32 Index = 4; Index < MatureSite.Num(); ++Index)
	{
		MatureSite[Index]->SetSpriteColor(
			bIncidentActive
				? FLinearColor(0.72f, 0.82f, 0.86f, 1.0f)
				: FLinearColor::White);
	}
	SetMatureInfrastructureTint(
		bIncidentActive
			? FLinearColor(0.68f, 0.78f, 0.83f, 1.0f)
			: FLinearColor::White);
	for (UPaperSpriteComponent* Component : MatureLandscapeClusters)
	{
		Component->SetSpriteColor(
			bIncidentActive
				? FLinearColor(0.48f, 0.62f, 0.61f, 1.0f)
				: FLinearColor::White);
	}
	const bool bRunwayClosed =
		bIncidentActive && State.Incident.bRunwayClosed;
	IncidentOverlay->SetRelativeLocation(
		FVector(24000.0, -21000.0, 84.0));
	IncidentOverlay->SetRelativeScale3D(FVector(12.0, 1.0, 12.0));
	IncidentOverlay->SetSpriteColor(
		FLinearColor(0.96f, 0.25f, 0.18f, 0.92f));
	IncidentOverlay->SetVisibility(bRunwayClosed);
	Phase4AffectedAircraft->SetVisibility(bIncidentActive);
	for (UPaperSpriteComponent* Component : Phase4ClosureHatch)
	{
		Component->SetVisibility(bRunwayClosed);
	}
	for (UPaperSpriteComponent* Component : Phase4RunwayClosure)
	{
		Component->SetVisibility(bRunwayClosed);
	}
	const bool bRouteVisible =
		bIncidentActive && State.Incident.bTowDispatched;
	for (UPaperSpriteComponent* Component : Phase4EmergencyRoute)
	{
		Component->SetVisibility(bRouteVisible);
	}
	for (UPaperSpriteComponent* Component : Phase4EmergencyRouteSegments)
	{
		Component->SetVisibility(bRouteVisible);
	}
	for (int32 Index = 0; Index < Phase4ResponseVehicles.Num(); ++Index)
	{
		const bool bVisible =
			bIncidentActive &&
			(Index == 0
				? State.Incident.bTowDispatched
				: State.Incident.bAreaProtected);
		Phase4ResponseVehicles[Index]->SetVisibility(bVisible);
	}
	Phase4ProtectionZone->SetVisibility(
		bIncidentActive && State.Incident.bAreaProtected);
	Terrain->SetSpriteColor(
		bIncidentActive
			? FLinearColor(0.11f, 0.20f, 0.20f, 1.0f)
			: FLinearColor(0.20f, 0.30f, 0.23f, 1.0f));
	RefreshIncidentPresentationVisibility();
}

void AAMSimWorldPresenter::SetPhase3OverlayMode(const int32 Mode)
{
	Phase3OverlayMode = FMath::Clamp(Mode, 0, 4);
	RefreshPhase3OverlayVisibility();
}

void AAMSimWorldPresenter::RefreshPhase3OverlayVisibility()
{
	const bool bRouteReady =
		bPhase3WorldVisible &&
		bPhase3RoutesConnected &&
		!bMatureOverviewMode;
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
		Phase3BaggageExceptionRoute,
		bAll || Phase3OverlayMode == 3);
	SetFamilyVisible(
		Phase3AccessibleFlow,
		bAll || Phase3OverlayMode == 4);
	SetFamilyVisible(
		Phase3AccessibleDashes,
		bAll || Phase3OverlayMode == 4);
	for (UPaperSpriteComponent* Component : Phase3Congestion)
	{
		Component->SetVisibility(bRouteReady && (bAll || Phase3OverlayMode == 1));
	}
	const bool bShowExceptionBranch =
		bRouteReady && (bAll || Phase3OverlayMode == 3);
	Phase3BaggageExceptionZone->SetVisibility(bShowExceptionBranch);
	Phase3BaggageExceptionStation->SetVisibility(bShowExceptionBranch);
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

int32 AAMSimWorldPresenter::GetActivePhase4ResponseProxyCount() const
{
	return Algo::CountIf(
		Phase4ResponseVehicles,
		[](const UPaperSpriteComponent* Component)
			{
				return Component && Component->IsVisible();
			});
}

int32 AAMSimWorldPresenter::GetActiveMatureSiteProxyCount() const
{
	int32 Count = Algo::CountIf(
		MatureSite,
		[](const UPaperSpriteComponent* Component)
			{
				return Component && Component->IsVisible();
			});
	const auto CountProcedural = [](const auto& Components)
	{
		return Algo::CountIf(
			Components,
			[](const UAMSimProceduralSurfaceComponent* Component)
			{
				return Component && Component->IsVisible();
			});
	};
	for (const UAMSimProceduralSurfaceComponent* Component : {
		MatureRunway.Get(),
		MatureTaxiway.Get(),
		MatureApron.Get(),
		MatureAccessRoad.Get()})
	{
		Count += Component && Component->IsVisible() ? 1 : 0;
	}
	return Count + CountProcedural(MatureGatePads) +
		CountProcedural(MatureTaxiConnectors) +
		CountProcedural(MatureLandsideLinks);
}
