#include "AMSimWorldPresenter.h"

#include "Algo/Count.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

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
}

void AAMSimWorldPresenter::FinalizePhase5Presentation(
	UPaperSprite* FeederFreighterSprite,
	UPaperSprite* RegionalFreighterSprite)
{
	const FVector ZoneLocations[] = {
		FVector(22000.0, 25500.0, 42.0),
		FVector(36000.0, 25500.0, 43.0),
		FVector(22000.0, 38000.0, 44.0),
		FVector(36000.0, 38000.0, 45.0)};
	const FLinearColor ZoneColors[] = {
		FLinearColor(0.16f, 0.31f, 0.34f, 1.0f),
		FLinearColor(0.12f, 0.48f, 0.56f, 1.0f),
		FLinearColor(0.22f, 0.58f, 0.70f, 1.0f),
		FLinearColor(0.55f, 0.36f, 0.15f, 1.0f)};
	for (int32 Index = 0; Index < Phase5WarehouseZones.Num(); ++Index)
	{
		ConfigureSprite(
			Phase5WarehouseZones[Index],
			WhiteSprite,
			ZoneLocations[Index],
			FVector(18.0f, 1.0f, 13.0f));
		Phase5WarehouseZones[Index]->SetSpriteColor(ZoneColors[Index]);
		Phase5WarehouseZones[Index]->SetVisibility(false);
	}

	for (int32 Index = 0; Index < Phase5CargoStacks.Num(); ++Index)
	{
		const int32 Column = Index % 4;
		const int32 Row = Index / 4;
		ConfigureSprite(
			Phase5CargoStacks[Index],
			WhiteSprite,
			FVector(
				17800.0 + Column * 4200.0,
				23000.0 + Row * 4100.0,
				59.0 + Index % 3),
			FVector(2.8f, 1.0f, 2.1f));
		Phase5CargoStacks[Index]->SetSpriteColor(
			Index % 3 == 0
				? FLinearColor(0.94f, 0.63f, 0.16f, 1.0f)
				: FLinearColor(0.60f, 0.39f, 0.19f, 1.0f));
		Phase5CargoStacks[Index]->SetVisibility(false);
	}

	for (int32 Index = 0; Index < Phase5CargoVehicles.Num(); ++Index)
	{
		ConfigureSprite(
			Phase5CargoVehicles[Index],
			WhiteSprite,
			FVector(
				11000.0 + Index * 4800.0,
				14500.0 + (Index % 2) * 2500.0,
				68.0 + Index % 3),
			FVector(
				Index == 2 ? 4.8f : 2.8f,
				1.0f,
				Index == 2 ? 2.2f : 1.8f));
		Phase5CargoVehicles[Index]->SetSpriteColor(
			Index == 2
				? FLinearColor(0.10f, 0.70f, 0.78f, 1.0f)
				: FLinearColor(0.96f, 0.67f, 0.17f, 1.0f));
		Phase5CargoVehicles[Index]->SetVisibility(false);
	}

	for (int32 Index = 0; Index < Phase5CargoRoutes.Num(); ++Index)
	{
		ConfigureSprite(
			Phase5CargoRoutes[Index],
			WhiteSprite,
			FVector(
				-5000.0 + Index * 7000.0,
				10500.0 + Index * 2200.0,
				57.0),
			FVector(6.8f, 1.0f, 0.65f));
		Phase5CargoRoutes[Index]->SetRelativeRotation(
			FRotator(0.0f, 18.0f, SpritePlaneRoll));
		Phase5CargoRoutes[Index]->SetVisibility(false);
	}

	for (int32 Index = 0; Index < Phase5EventArea.Num(); ++Index)
	{
		const int32 Column = Index % 6;
		const int32 Row = Index / 6;
		ConfigureSprite(
			Phase5EventArea[Index],
			WhiteSprite,
			FVector(
				-28000.0 + Column * 2600.0,
				32000.0 + Row * 2500.0,
				72.0 + Index % 3),
			FVector(0.85f, 1.0f, 0.85f));
		Phase5EventArea[Index]->SetSpriteColor(
			Index % 2 == 0
				? FLinearColor(0.96f, 0.74f, 0.22f, 1.0f)
				: FLinearColor(0.15f, 0.75f, 0.83f, 1.0f));
		Phase5EventArea[Index]->SetVisibility(false);
	}

	const FVector FreighterLocations[] = {
		FVector(8000.0, 2500.0, 67.0),
		FVector(24000.0, 7000.0, 68.0)};
	UPaperSprite* FreighterSprites[] = {
		FeederFreighterSprite,
		RegionalFreighterSprite};
	for (int32 Index = 0; Index < Phase5Freighters.Num(); ++Index)
	{
		ConfigureSprite(
			Phase5Freighters[Index],
			FreighterSprites[Index],
			FreighterLocations[Index],
			FVector(Index == 0 ? 7.0f : 9.0f, 1.0f, Index == 0 ? 7.0f : 9.0f));
		Phase5Freighters[Index]->SetRelativeRotation(
			FRotator(0.0f, Index == 0 ? 90.0f : 270.0f, SpritePlaneRoll));
		Phase5Freighters[Index]->SetVisibility(false);
	}
}

void AAMSimWorldPresenter::ApplyPhase5Snapshot(
	const AMSim::FPhase5QuerySnapshot& Query,
	const AMSim::FPhase5State& State)
{
	if (LastAppliedPhase5Revision == Query.Revision)
	{
		return;
	}
	LastAppliedPhase5Revision = Query.Revision;
	if (!Query.bInitialized)
	{
		SetPoolVisible(Phase5WarehouseZones, 0);
		SetPoolVisible(Phase5CargoStacks, 0);
		SetPoolVisible(Phase5CargoVehicles, 0);
		SetPoolVisible(Phase5CargoRoutes, 0);
		SetPoolVisible(Phase5EventArea, 0);
		SetPoolVisible(Phase5Freighters, 0);
		return;
	}

	SetPoolVisible(
		Phase5WarehouseZones,
		FMath::Min(State.WarehouseZones.Num(), Phase5WarehouseZones.Num()));
	const int32 CargoStackCount = FMath::Min(
		Query.ActiveShipmentCount * 2,
		Phase5CargoStacks.Num());
	SetPoolVisible(Phase5CargoStacks, CargoStackCount);
	SetPoolVisible(
		Phase5CargoVehicles,
		FMath::Min(Query.ActiveShipmentCount, Phase5CargoVehicles.Num()));
	SetPoolVisible(
		Phase5CargoRoutes,
		Query.ActiveShipmentCount > 0 ? Phase5CargoRoutes.Num() : 0);
	SetPoolVisible(
		Phase5EventArea,
		Query.ActiveEventCount > 0 ? Phase5EventArea.Num() : 0);
	const int32 FreighterCount = State.Shipments.ContainsByPredicate(
		[](const AMSim::FPhase5ShipmentRecord& Shipment)
			{
				return !Shipment.bBellyFreight &&
					Shipment.State != AMSim::ECargoState::Offered &&
					Shipment.State != AMSim::ECargoState::Completed;
			})
		? FMath::Min(2, Query.ActiveShipmentCount)
		: 0;
	SetPoolVisible(Phase5Freighters, FreighterCount);

	for (int32 Index = 0; Index < Phase5Freighters.Num(); ++Index)
	{
		if (Phase5Freighters[Index]->IsVisible())
		{
			const float Heading =
				22.5f * static_cast<float>(
					(Query.OperatingDay + Index * 4) % 16);
			Phase5Freighters[Index]->SetRelativeRotation(
				FRotator(0.0f, Heading, SpritePlaneRoll));
		}
	}
}

int32 AAMSimWorldPresenter::GetActivePhase5CargoProxyCount() const
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
	return CountVisible(Phase5WarehouseZones) +
		CountVisible(Phase5CargoStacks) +
		CountVisible(Phase5CargoVehicles) +
		CountVisible(Phase5CargoRoutes) +
		CountVisible(Phase5Freighters);
}
