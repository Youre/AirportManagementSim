#pragma once

#include "AMSimPhase1Types.h"
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
	uint64 GetLastAppliedRevision() const { return LastAppliedRevision; }
	bool HasRequiredPresentationAssets() const;
	static int32 GetHeadingIndex(AMSim::EFlightState FlightState);

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
};

