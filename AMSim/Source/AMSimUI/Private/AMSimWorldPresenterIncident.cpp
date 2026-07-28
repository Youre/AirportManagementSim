#include "AMSimWorldPresenter.h"

#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

void AAMSimWorldPresenter::FinalizeIncidentPresentation(
	UPaperSprite* RunwayAsphaltSprite,
	UPaperSprite* TaxiwayAsphaltSprite,
	UPaperSprite* ApronStandSprite)
{
	const UPaperSprite* WetSurfaceSprites[] = {
		RunwayAsphaltSprite,
		TaxiwayAsphaltSprite,
		ApronStandSprite};
	const FVector WetSurfaceLocations[] = {
		FVector(32000.0, 0.0, 48.0),
		FVector(21000.0, 0.0, 49.0),
		FVector(7000.0, 0.0, 50.0)};
	const FVector WetSurfaceScales[] = {
		FVector(80.0, 1.0, 12.2),
		FVector(74.0, 1.0, 9.2),
		FVector(32.5, 1.0, 28.5)};
	const FLinearColor WetSurfaceTints[] = {
		FLinearColor(0.58f, 0.82f, 0.92f, 0.38f),
		FLinearColor(0.48f, 0.75f, 0.88f, 0.34f),
		FLinearColor(0.44f, 0.68f, 0.80f, 0.30f)};
	static_assert(
		UE_ARRAY_COUNT(WetSurfaceLocations) ==
			UE_ARRAY_COUNT(WetSurfaceScales));
	static_assert(
		UE_ARRAY_COUNT(WetSurfaceLocations) ==
			UE_ARRAY_COUNT(WetSurfaceTints));

	for (int32 Index = 0; Index < Phase4WetSurfaces.Num(); ++Index)
	{
		ConfigureSprite(
			Phase4WetSurfaces[Index],
			const_cast<UPaperSprite*>(WetSurfaceSprites[Index]),
			WetSurfaceLocations[Index],
			WetSurfaceScales[Index]);
		Phase4WetSurfaces[Index]->SetSpriteColor(WetSurfaceTints[Index]);
		Phase4WetSurfaces[Index]->SetVisibility(false);
	}

	constexpr float SpritePlaneRoll = -90.0f;
	const FQuat FlatOrientation =
		FRotator(0.0f, 0.0f, SpritePlaneRoll).Quaternion();
	const FQuat LocalInPlaneRotation(
		FVector::YAxisVector,
		FMath::DegreesToRadians(90.0f));
	for (int32 Index = 0; Index < 2; ++Index)
	{
		Phase4WetSurfaces[Index]->SetRelativeRotation(
			(FlatOrientation * LocalInPlaneRotation).Rotator());
	}
}
