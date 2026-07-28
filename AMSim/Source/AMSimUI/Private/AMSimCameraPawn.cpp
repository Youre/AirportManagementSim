#include "AMSimCameraPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputCoreTypes.h"

AAMSimCameraPawn::AAMSimCameraPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("OrthographicCamera"));
	Camera->SetupAttachment(Root);
	Camera->SetRelativeLocation({0.0, 0.0, 2000.0});
	Camera->SetRelativeRotation({-90.0, 0.0, 0.0});
	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 105000.0f;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	MappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("CameraInput"));
	PanUpAction = CreateDefaultSubobject<UInputAction>(TEXT("PanUp"));
	PanDownAction = CreateDefaultSubobject<UInputAction>(TEXT("PanDown"));
	PanLeftAction = CreateDefaultSubobject<UInputAction>(TEXT("PanLeft"));
	PanRightAction = CreateDefaultSubobject<UInputAction>(TEXT("PanRight"));
	ZoomAction = CreateDefaultSubobject<UInputAction>(TEXT("Zoom"));
	ZoomAction->ValueType = EInputActionValueType::Axis1D;
	MappingContext->MapKey(PanUpAction, EKeys::W);
	MappingContext->MapKey(PanDownAction, EKeys::S);
	MappingContext->MapKey(PanLeftAction, EKeys::A);
	MappingContext->MapKey(PanRightAction, EKeys::D);
	MappingContext->MapKey(ZoomAction, EKeys::MouseWheelAxis);
}

void AAMSimCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	ManagementCameraLocation = GetActorLocation();
	ManagementOrthoWidth = Camera->OrthoWidth;
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AAMSimCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* Enhanced = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Enhanced->BindAction(PanUpAction, ETriggerEvent::Triggered, this, &AAMSimCameraPawn::PanUp);
		Enhanced->BindAction(PanDownAction, ETriggerEvent::Triggered, this, &AAMSimCameraPawn::PanDown);
		Enhanced->BindAction(PanLeftAction, ETriggerEvent::Triggered, this, &AAMSimCameraPawn::PanLeft);
		Enhanced->BindAction(PanRightAction, ETriggerEvent::Triggered, this, &AAMSimCameraPawn::PanRight);
		Enhanced->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AAMSimCameraPawn::Zoom);
	}
}

void AAMSimCameraPawn::PanUp() { Pan(FVector::ForwardVector); }
void AAMSimCameraPawn::PanDown() { Pan(-FVector::ForwardVector); }
void AAMSimCameraPawn::PanLeft() { Pan(-FVector::RightVector); }
void AAMSimCameraPawn::PanRight() { Pan(FVector::RightVector); }

void AAMSimCameraPawn::Pan(const FVector Direction)
{
	if (bCloseOperationsMode)
	{
		return;
	}
	AddActorWorldOffset(Direction * 5000.0);
}

void AAMSimCameraPawn::Zoom(const FInputActionValue& Value)
{
	if (bCloseOperationsMode)
	{
		return;
	}
	Camera->OrthoWidth = FMath::Clamp(
		Camera->OrthoWidth - Value.Get<float>() * 5000.0f,
		30000.0f,
		160000.0f);
}

void AAMSimCameraPawn::SetCloseOperationsMode(const bool bEnabled)
{
	if (bEnabled == bCloseOperationsMode)
	{
		return;
	}
	if (bEnabled)
	{
		ManagementCameraLocation = GetActorLocation();
		ManagementOrthoWidth = Camera->OrthoWidth;
		const FVector Current = GetActorLocation();
		SetActorLocation(FVector(-32000.0, 23000.0, Current.Z));
		Camera->OrthoWidth = 30000.0f;
	}
	else
	{
		SetActorLocation(ManagementCameraLocation);
		Camera->OrthoWidth = ManagementOrthoWidth;
	}
	bCloseOperationsMode = bEnabled;
}
