#include "AMSimWorldPresenter.h"

#include "Algo/Count.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

void AAMSimWorldPresenter::FinalizePhase1OperationsPresentation(
	UPaperSprite* ConstructionTruckSprite,
	UPaperSprite* ConstructionWorkerSprite,
	UPaperSprite* SafetyConesSprite,
	UPaperSprite* ProtectionZoneSprite,
	UPaperSprite* FuelTruckSprite,
	UPaperSprite* RampWorkerSprite,
	UPaperSprite* DirectionArrowSprite)
{
	UPaperSprite* ConstructionSprites[] = {
		ConstructionTruckSprite,
		ConstructionWorkerSprite,
		SafetyConesSprite,
		ProtectionZoneSprite};
	const FVector ConstructionLocations[] = {
		FVector(-12000.0, 9000.0, 57.0),
		FVector(-6500.0, 8500.0, 56.0),
		FVector(-9000.0, 3500.0, 55.0),
		FVector(-12000.0, 9000.0, 54.0)};
	const FVector ConstructionScales[] = {
		FVector(10.0, 1.0, 10.0),
		FVector(5.0, 1.0, 5.0),
		FVector(7.0, 1.0, 7.0),
		FVector(7.0, 1.0, 6.0)};
	for (int32 Index = 0; Index < Phase1ConstructionProxies.Num(); ++Index)
	{
		ConfigureSprite(
			Phase1ConstructionProxies[Index],
			ConstructionSprites[Index],
			ConstructionLocations[Index],
			ConstructionScales[Index]);
		Phase1ConstructionProxies[Index]->SetVisibility(false);
	}

	ConfigureSprite(
		TurnaroundFuelTruck,
		FuelTruckSprite,
		FVector(-36500.0, 26500.0, 73.0),
		FVector(5.0, 1.0, 5.0));
	ConfigureSprite(
		TurnaroundRampWorker,
		RampWorkerSprite,
		FVector(-28500.0, 19000.0, 74.0),
		FVector(3.2, 1.0, 3.2));
	ConfigureSprite(
		TurnaroundSafetyCones,
		SafetyConesSprite,
		FVector(-32000.0, 28000.0, 71.0),
		FVector(3.5, 1.0, 3.5));
	ConfigureSprite(
		TurnaroundSafetyZone,
		ProtectionZoneSprite,
		FVector(-36500.0, 26500.0, 68.0),
		FVector(5.8, 1.0, 5.2));

	const FVector ApproachLocations[] = {
		FVector(-38500.0, 25000.0, 69.0),
		FVector(-30000.0, 17500.0, 70.0)};
	const FRotator ApproachRotations[] = {
		FRotator(0.0, -35.0, -90.0),
		FRotator(0.0, 145.0, -90.0)};
	for (int32 Index = 0; Index < TurnaroundApproachPaths.Num(); ++Index)
	{
		ConfigureSprite(
			TurnaroundApproachPaths[Index],
			DirectionArrowSprite,
			ApproachLocations[Index],
			FVector(3.0, 1.0, 3.0));
		TurnaroundApproachPaths[Index]->SetRelativeRotation(
			ApproachRotations[Index]);
		TurnaroundApproachPaths[Index]->SetVisibility(false);
	}

	TurnaroundFuelTruck->SetVisibility(false);
	TurnaroundRampWorker->SetVisibility(false);
	TurnaroundSafetyCones->SetVisibility(false);
	TurnaroundSafetyZone->SetVisibility(false);
}

void AAMSimWorldPresenter::RefreshPhase1OperationsPresentation(
	const AMSim::FPhase1QuerySnapshot& Query)
{
	const bool bAwaitingDelivery =
		Query.ConstructionStage == AMSim::EConstructionStage::Funded ||
		Query.ConstructionStage == AMSim::EConstructionStage::AwaitingDelivery;
	const bool bBuilding =
		Query.ConstructionStage == AMSim::EConstructionStage::Building;
	const bool bInspection =
		Query.ConstructionStage == AMSim::EConstructionStage::Inspection;
	const bool ConstructionVisibility[] = {
		bAwaitingDelivery || bBuilding,
		bBuilding || bInspection,
		bAwaitingDelivery || bBuilding || bInspection,
		bBuilding || bInspection};
	for (int32 Index = 0; Index < Phase1ConstructionProxies.Num(); ++Index)
	{
		Phase1ConstructionProxies[Index]->SetVisibility(
			ConstructionVisibility[Index]);
	}

	const bool bTurnaround =
		Query.FlightState == AMSim::EFlightState::Turnaround;
	const bool bInspectionActive =
		Query.InspectionState == AMSim::EServiceTaskState::Active;
	const bool bFuelingActive =
		Query.FuelingState == AMSim::EServiceTaskState::Active;
	InspectionMarker->SetVisibility(bTurnaround && bInspectionActive);
	FuelMarker->SetVisibility(bTurnaround && bFuelingActive);
	TurnaroundFuelTruck->SetVisibility(bTurnaround && bFuelingActive);
	TurnaroundRampWorker->SetVisibility(bTurnaround && bInspectionActive);
	TurnaroundSafetyCones->SetVisibility(bTurnaround);
	TurnaroundSafetyZone->SetVisibility(bTurnaround);
	for (int32 Index = 0; Index < TurnaroundApproachPaths.Num(); ++Index)
	{
		TurnaroundApproachPaths[Index]->SetVisibility(
			bTurnaround &&
			(Index == 0 ? bFuelingActive : bInspectionActive));
	}
}

int32 AAMSimWorldPresenter::GetActivePhase1ConstructionProxyCount() const
{
	return Algo::CountIf(
		Phase1ConstructionProxies,
		[](const UPaperSpriteComponent* Component)
		{
			return Component && Component->IsVisible();
		});
}

int32 AAMSimWorldPresenter::GetActiveTurnaroundSupportProxyCount() const
{
	int32 Count = Algo::CountIf(
		TurnaroundApproachPaths,
		[](const UPaperSpriteComponent* Component)
		{
			return Component && Component->IsVisible();
		});
	for (const UPaperSpriteComponent* Component : {
		TurnaroundFuelTruck.Get(),
		TurnaroundRampWorker.Get(),
		TurnaroundSafetyCones.Get(),
		TurnaroundSafetyZone.Get()})
	{
		Count += Component && Component->IsVisible() ? 1 : 0;
	}
	return Count;
}
