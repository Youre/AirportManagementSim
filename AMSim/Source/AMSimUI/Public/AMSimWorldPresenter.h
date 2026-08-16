#pragma once

#include "AMSimPhase1Types.h"
#include "AMSimPhase2Types.h"
#include "AMSimPhase3Types.h"
#include "AMSimPhase4Types.h"
#include "AMSimPhase5Types.h"
#include "AMSimPhase6Types.h"
#include "AMSimPhase1ConstructionPreview.h"
#include "GameFramework/Actor.h"
#include "AMSimWorldPresenter.generated.h"

class UPaperSprite;
class UPaperSpriteComponent;
class UMaterialInterface;
class UTextRenderComponent;
class USceneComponent;

UCLASS()
class AMSIMUI_API AAMSimWorldPresenter final : public AActor
{
	GENERATED_BODY()

public:
	AAMSimWorldPresenter();

	void ApplySnapshot(
		const AMSim::FPhase1QuerySnapshot& Query,
		const AMSim::FPhase1State& State);
	void ApplyPhase2Snapshot(
		const AMSim::FPhase2QuerySnapshot& Query,
		const AMSim::FPhase2State& State);
	void ApplyPhase3Snapshot(
		const AMSim::FPhase3QuerySnapshot& Query,
		const AMSim::FPhase3State& State);
	void ApplyPhase4Snapshot(
		const AMSim::FPhase4QuerySnapshot& Query,
		const AMSim::FPhase4State& State);
	void ApplyPhase5Snapshot(
		const AMSim::FPhase5QuerySnapshot& Query,
		const AMSim::FPhase5State& State);
	void ApplyPhase6Snapshot(
		const AMSim::FPhase6QuerySnapshot& Query,
		const AMSim::FPhase6State& State);
	void SetPhase1OverlayMode(int32 Mode);
	void SetPhase3OverlayMode(int32 Mode);
	void SetTerminalCutawayMode(bool bEnabled);
	bool IsTerminalCutawayMode() const { return bTerminalCutawayMode; }
	FVector GetTerminalWorldCenter() const;
	int32 GetActiveTerminalFloorProxyCount() const;
	int32 GetActiveTerminalRoofProxyCount() const;
	int32 GetActiveTerminalConstructionProxyCount() const;
	int32 GetActiveTerminalPlacementPreviewCount() const;
	void SetTerminalPlacementPreview(
		const AMSim::FTerminalCellCoord& Start,
		const AMSim::FTerminalCellCoord& End,
		int32 VisualState);
	void ClearTerminalPlacementPreview();
	int32 GetAllocatedTerminalFloorProxyCount() const
	{
		return TerminalLayoutFloorProxies.Num();
	}
	bool IsLegacyStarterTerminalVisibleForTest() const;
	void SetMatureOverviewMode(bool bEnabled);
	void SetMatureSelectionFacility(bool bFacilitySelected);
	void SetConstructionEditorOverlayVisible(bool bVisible);
	void SetPhase1ConstructionPreview(
		const AMSim::FPhase1ConstructionPreviewState& Preview);
	void ClearPhase1ConstructionPreview();
	int32 GetActivePhase1ConstructionPreviewSurfaceCount() const;
	int32 GetActivePhase1ConstructionPreviewMarkerCount() const;
	int32 GetActivePhase1ConstructionPreviewPatternCount() const;
	int32 GetActivePhase1ConstructionPlanningGridCount() const;
	bool HasPhase1ConstructionPreviewMarkerAssets() const;
	bool ArePhase1ConstructionPreviewSurfacesBelowStarterFacilities() const;
	bool ArePhase1ConstructionPreviewMarkersAboveStarterFacilities() const;
	FVector GetPhase1ConstructionPreviewRunwayCenterForTest() const;
	bool IsStarterContextVisibleForTest() const;
	uint64 GetLastAppliedRevision() const { return LastAppliedRevision; }
	bool HasRequiredPresentationAssets() const;
	bool HasDistinctStarterFacilityAssets() const;
	FVector GetStarterTerminalScaleForTest() const;
	FVector GetStarterGateScaleForTest() const;
	float GetStarterTerminalYawForTest() const;
	float GetStarterGateYawForTest() const;
	bool HasDistinctPhase1MovementSurfaceAssets() const
	{
		return RunwaySprite && TaxiSprite && AccessSprite &&
			RunwaySprite != TaxiSprite && TaxiSprite != AccessSprite;
	}
	static int32 GetHeadingIndex(AMSim::EFlightState FlightState);
	static int32 GetPhase2HeadingIndex(AMSim::EPhase2FlightState FlightState);
	int32 GetActivePhase2AircraftProxyCount() const;
	int32 GetActivePhase2VehicleProxyCount() const;
	int32 GetActivePhase3PassengerProxyCount() const;
	int32 GetActivePhase3BagProxyCount() const;
	int32 GetActivePhase4ResponseProxyCount() const;
	int32 GetActivePhase5CargoProxyCount() const;
	int32 GetActivePhase6ProxyCount() const;
	int32 GetActiveMatureSiteProxyCount() const;
	int32 GetActivePhase1ConstructionProxyCount() const;
	int32 GetActivePhase1EarthworkProxyCount() const;
	bool ArePhase1ConstructionBedsBelowSurfaces() const;
	int32 GetActiveTurnaroundSupportProxyCount() const;
	float GetPhase1ConstructionTravelProgress() const
	{
		return Phase1ConstructionTravelProgress;
	}
	float GetPhase1ConstructionSurfaceProgress() const
	{
		return Phase1ConstructionSurfaceProgress;
	}
	FVector GetPhase1ConstructionWorkerLocation(int32 WorkerIndex) const;
	float GetPhase1ConstructionWorkerFacingYawDegrees(int32 WorkerIndex) const;
	float GetPhase1ConstructionTruckFacingYawDegrees() const;
	int32 GetTerminalAccessibleDashProxyCount() const
	{
		return Phase3AccessibleDashes.Num();
	}
	int32 GetIncidentRouteSegmentProxyCount() const
	{
		return Phase4EmergencyRouteSegments.Num();
	}
	int32 GetTerminalExceptionRouteProxyCount() const
	{
		return Phase3BaggageExceptionRoute.Num();
	}
	int32 GetWetSurfaceProxyCount() const
	{
		return Phase4WetSurfaces.Num();
	}
	bool HasMatureSelectionProxy() const
	{
		return MatureSelection != nullptr;
	}
	bool IsPhase4IncidentWorldVisible() const
	{
		return bPhase4IncidentWorldVisible;
	}
	FVector GetPhase1GeometryOffset() const { return Phase1GeometryOffset; }
	FVector GetPhase1RunwayCenter() const { return Phase1RunwayCenter; }
	int32 GetPhase1OverlayMode() const { return Phase1OverlayMode; }
	FLinearColor GetPhase1RunwayTintForTest() const;

private:
	UPaperSpriteComponent* CreateSpriteComponent(
		const TCHAR* Name,
		int32 SortPriority,
		const FLinearColor& Color = FLinearColor::White);
	void ConfigureSprite(
		UPaperSpriteComponent* Component,
		UPaperSprite* Sprite,
		const FVector& Location,
		const FVector& Scale);
	void SetFacilitiesVisible(
		bool bNetworkVisible,
		bool bOperational,
		bool bStarterContextVisible = false);
	void SetAircraftState(
		const AMSim::FPhase1QuerySnapshot& Query,
		const AMSim::FPhase1State& State);
	void ApplyPhase1Geometry(const AMSim::FStarterPlanProposal& Proposal);
	void InitializePhase1ConstructionPresentation();
	void InitializePhase1ConstructionPreviewPresentation();
	void LoadPhase1ConstructionPreviewAssets();
	void FinalizePhase1ConstructionPreviewPresentation();
	void FinalizePhase1OperationsPresentation(
		UPaperSprite* ConstructionTruckSprite,
		UPaperSprite* ConstructionWorkerSprite,
		UPaperSprite* SafetyConesSprite,
		UPaperSprite* ProtectionZoneSprite,
		UPaperSprite* FuelTruckSprite,
		UPaperSprite* RampWorkerSprite,
		UPaperSprite* DirectionArrowSprite);
	void RefreshPhase1OperationsPresentation(
		const AMSim::FPhase1QuerySnapshot& Query,
		const AMSim::FPhase1State& State);
	void RefreshPhase1ConstructionPresentation(
		const AMSim::FPhase1QuerySnapshot& Query,
		const AMSim::FPhase1State& State);
	void RefreshPhase1OverlayPresentation();
	void SetPhase3WorldVisible(bool bVisible);
	void RefreshPhase3OverlayVisibility();
	void RefreshIncidentPresentationVisibility();
	void FinalizeTerminalPresentation(
		UPaperSprite* DirectionArrowSprite,
		UPaperSprite* AccessibleRouteSprite,
		UPaperSprite* SecureDoorSprite,
		UPaperSprite* PartitionWallSprite,
		UPaperSprite* CautionHatchSprite,
		UPaperSprite* SortingTableSprite);
	void FinalizeIncidentPresentation(
		UPaperSprite* RunwayAsphaltSprite,
		UPaperSprite* TaxiwayAsphaltSprite,
		UPaperSprite* ApronStandSprite);
	void FinalizePhase5Presentation(
		UPaperSprite* FeederFreighterSprite,
		UPaperSprite* RegionalFreighterSprite);
	void InitializePhase6Presentation();
	void InitializeTerminalLayoutPresentation();
	void RefreshTerminalLayoutPresentation(
		const AMSim::FTerminalLayoutQuerySnapshot& Query,
		const AMSim::FPhase3State& State,
		bool bForce = false);
	UPaperSpriteComponent* AcquireTerminalLayoutProxy(
		TArray<TObjectPtr<UPaperSpriteComponent>>& Pool,
		int32 Index,
		const TCHAR* Prefix,
		int32 SortPriority);
	void HideTerminalLayoutPool(
		TArray<TObjectPtr<UPaperSpriteComponent>>& Pool,
		int32 FirstUnusedIndex);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Terrain;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Runway;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Taxiway;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1TaxiwaySegments;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Stand;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> GateB;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Access;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> OperationsHut;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Windsock;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> RunwayStartNumber;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> RunwayEndNumber;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Aircraft;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Selection;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> InspectionMarker;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> FuelMarker;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> TurnaroundFuelTruck;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> TurnaroundRampWorker;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> TurnaroundSafetyCones;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> TurnaroundSafetyZone;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TurnaroundApproachPaths;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1ConstructionProxies;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase1RunwayEarthwork;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1TaxiwayEarthworks;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase1RoadEarthwork;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1ConstructionCrew;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1ConstructionBoundaryMarkers;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase1ConstructionPreviewRunway;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1ConstructionPreviewTaxiways;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase1ConstructionPreviewRoad;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1ConstructionPreviewOutlines;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1ConstructionPreviewPattern;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1ConstructionPlanningGrid;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase1ConstructionPreviewMarkers;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase2Aircraft;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase2Vehicles;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> ExpansionOverlay;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> ExpansionClosureOverlay;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> IncidentOverlay;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3Floor;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3Rooms;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3DepartureFlow;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3ArrivalFlow;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3LandsideFlow;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3BaggageFlow;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3AccessibleFlow;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3AccessibleDashes;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3SecurityBoundary;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3BaggageExceptionRoute;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase3BaggageExceptionZone;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase3BaggageExceptionStation;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3Congestion;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3Passengers;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3Bags;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3Vehicles;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3Staff;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase3Aircraft;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3Props;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TerminalLayoutFloorProxies;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TerminalLayoutRoofProxies;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TerminalLayoutRoofDetailProxies;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TerminalLayoutEdgeProxies;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TerminalLayoutObjectProxies;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TerminalLayoutConstructionProxies;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TerminalLayoutWorkerProxies;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TerminalLayoutVisitorProxies;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> TerminalPlacementPreviewProxies;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> MatureSite;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> MatureTaxiConnectors;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> MatureLandsideLinks;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> MatureLandscapeClusters;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> MatureAircraft;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> MatureSelection;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> MatureGroundVehicles;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> MaturePeople;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> MatureBags;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase4WeatherOverlay;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase4WetSurfaces;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase4RainStreaks;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase4IncidentRunway;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase4AffectedAircraft;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase4ClosureHatch;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase4RunwayClosure;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase4EmergencyRoute;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase4EmergencyRouteSegments;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase4ResponseVehicles;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Phase4ProtectionZone;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase5WarehouseZones;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase5CargoStacks;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase5CargoVehicles;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase5CargoRoutes;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase5EventArea;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase5Freighters;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase6RunwaySystem;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase6Facilities;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase6LargeAircraft;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase6Response;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase6Closure;

	UPROPERTY()
	TObjectPtr<UPaperSprite> TerrainSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> RunwaySprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> TaxiSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> AccessSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> StandSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> WhiteSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> HutSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> WindsockSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> SelectionSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> ConstructionSnapSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> ConstructionConnectedSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> ConstructionCrossingSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> ConstructionInvalidSprite;
	UPROPERTY()
	TObjectPtr<UMaterialInterface> ConstructionPreviewMaterial;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> AircraftHeadingSprites;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> OperationsSprites;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> TerminalSprites;
	UPROPERTY()
	TObjectPtr<UPaperSprite> TerminalPublicFloorSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> TerminalServiceFloorSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> TerminalRoofSurfaceSprite;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> TerminalRoofDetailSprites;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> TerminalEdgeSprites;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> TerminalObjectSprites;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> TerminalConstructionSprites;
	UPROPERTY()
	TObjectPtr<UPaperSprite> TerminalConstructionWorkerSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> TerminalVisitorSprite;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> TerminalPlacementOverlaySprites;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> SiteSprites;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> Phase5FreighterSprites;
	UPROPERTY()
	TArray<TObjectPtr<UPaperSprite>> Phase6WidebodySprites;

	uint64 LastAppliedRevision = MAX_uint64;
	uint64 LastAppliedPhase2Revision = MAX_uint64;
	uint64 LastAppliedPhase3Revision = MAX_uint64;
	uint64 LastAppliedPhase4Revision = MAX_uint64;
	uint64 LastAppliedPhase5Revision = MAX_uint64;
	uint64 LastAppliedPhase6Revision = MAX_uint64;
	int32 Phase3OverlayMode = 0;
	int32 Phase1OverlayMode = 0;
	bool bPhase3WorldVisible = false;
	bool bPhase1AirportInitialized = false;
	bool bTerminalCutawayMode = false;
	uint64 LastAppliedTerminalLayoutRevision = MAX_uint64;
	AMSim::FTerminalLayoutQuerySnapshot CachedTerminalLayoutSnapshot;
	AMSim::FPhase3State CachedTerminalPhase3State;
	bool bPhase3RoutesConnected = false;
	bool bPhase3BaggageExceptionActive = false;
	bool bMatureOverviewMode = false;
	bool bMatureFacilitySelected = false;
	bool bIncidentPresentationMode = false;
	int32 Phase3PassengerAvailableCount = 0;
	int32 Phase3BagAvailableCount = 0;
	bool bPhase3AircraftAvailable = false;
	int32 MatureAircraftAvailableCount = 0;
	int32 MatureGroundVehicleAvailableCount = 0;
	int32 MaturePeopleAvailableCount = 0;
	int32 MatureBagAvailableCount = 0;
	bool bPhase4IncidentWorldVisible = false;
	bool bPhase4RunwayClosed = false;
	bool bPhase4RouteVisible = false;
	bool bPhase4ProtectionVisible = false;
	FVector Phase1GeometryOffset = FVector::ZeroVector;
	FVector Phase1RunwayCenter = FVector::ZeroVector;
	FVector Phase1TaxiCenter = FVector::ZeroVector;
	FVector Phase1StandCenter = FVector::ZeroVector;
	int32 ActivePhase1TaxiwaySegmentCount = 0;
	bool bPhase1RoadPresent = false;
	bool bRequestedPhase1NetworkVisible = false;
	bool bRequestedPhase1Operational = false;
	bool bRequestedStarterContextVisible = false;
	float Phase1ConstructionTravelProgress = 0.0f;
	float Phase1ConstructionSurfaceProgress = 0.0f;
};
