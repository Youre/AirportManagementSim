#pragma once

#include "GameFramework/Pawn.h"
#include "AMSimCameraPawn.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class AMSIMUI_API AAMSimCameraPawn final : public APawn
{
	GENERATED_BODY()

public:
	AAMSimCameraPawn();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UCameraComponent* GetCamera() const { return Camera; }
	void SetCloseOperationsMode(bool bEnabled);
	void PanByScreenDelta(const FVector2D& ScreenDelta);
	FVector GetManagementLocation() const { return GetActorLocation(); }
	static FVector CalculateScreenPanDelta(
		const FVector2D& ScreenDelta,
		float OrthoWidth);
	static float CalculateZoomedOrthoWidth(
		float CurrentOrthoWidth,
		float InputAmount);
	static FVector GetInitialManagementCameraOffset()
	{
		return FVector(-5000.0f, 0.0f, 0.0f);
	}

private:
	void PanUp();
	void PanDown();
	void PanLeft();
	void PanRight();
	void Zoom(const struct FInputActionValue& Value);
	void Pan(FVector Direction);
	void ClampManagementLocation();

	bool bCloseOperationsMode = false;
	FVector ManagementCameraLocation = FVector::ZeroVector;
	float ManagementOrthoWidth = 105000.0f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> PanUpAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> PanDownAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> PanLeftAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> PanRightAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> ZoomAction;
};
