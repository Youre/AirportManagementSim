#pragma once

#include "AMSimPhase1Types.h"
#include "AMSimPhase2Types.h"
#include "AMSimPhase3Types.h"
#include "GameFramework/Actor.h"
#include "AMSimWorldPresenter.generated.h"

class UPaperSprite;
class UPaperSpriteComponent;
class USceneComponent;

UCLASS()
class AMSIMUI_API AAMSimWorldPresenter final : public AActor
{
	GENERATED_BODY()

public:
	AAMSimWorldPresenter();

	void ApplySnapshot(const AMSim::FPhase1QuerySnapshot& Query);
	void ApplyPhase2Snapshot(
		const AMSim::FPhase2QuerySnapshot& Query,
		const AMSim::FPhase2State& State);
	void ApplyPhase3Snapshot(
		const AMSim::FPhase3QuerySnapshot& Query,
		const AMSim::FPhase3State& State);
	void SetPhase3OverlayMode(int32 Mode);
	uint64 GetLastAppliedRevision() const { return LastAppliedRevision; }
	bool HasRequiredPresentationAssets() const;
	static int32 GetHeadingIndex(AMSim::EFlightState FlightState);
	static int32 GetPhase2HeadingIndex(AMSim::EPhase2FlightState FlightState);
	int32 GetActivePhase2AircraftProxyCount() const;
	int32 GetActivePhase2VehicleProxyCount() const;
	int32 GetActivePhase3PassengerProxyCount() const;
	int32 GetActivePhase3BagProxyCount() const;

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
	void SetFacilitiesVisible(bool bVisible, bool bOperational);
	void SetAircraftState(const AMSim::FPhase1QuerySnapshot& Query);
	void SetPhase3WorldVisible(bool bVisible);
	void RefreshPhase3OverlayVisibility();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Terrain;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Runway;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Taxiway;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Stand;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Access;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> OperationsHut;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Windsock;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Aircraft;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> Selection;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> InspectionMarker;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> FuelMarker;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase2Aircraft;
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase2Vehicles;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> ExpansionOverlay;
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
	TArray<TObjectPtr<UPaperSpriteComponent>> Phase3SecurityBoundary;
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

	UPROPERTY()
	TObjectPtr<UPaperSprite> TerrainSprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> RunwaySprite;
	UPROPERTY()
	TObjectPtr<UPaperSprite> TaxiSprite;
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
	TArray<TObjectPtr<UPaperSprite>> AircraftHeadingSprites;

	uint64 LastAppliedRevision = MAX_uint64;
	uint64 LastAppliedPhase2Revision = MAX_uint64;
	uint64 LastAppliedPhase3Revision = MAX_uint64;
	int32 Phase3OverlayMode = 0;
	bool bPhase3WorldVisible = false;
	bool bPhase3RoutesConnected = false;
};
