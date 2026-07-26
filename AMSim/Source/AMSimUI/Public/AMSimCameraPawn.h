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

private:
	void PanUp();
	void PanDown();
	void PanLeft();
	void PanRight();
	void Zoom(const struct FInputActionValue& Value);
	void Pan(FVector Direction);

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
