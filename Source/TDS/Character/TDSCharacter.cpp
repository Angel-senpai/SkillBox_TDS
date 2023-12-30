// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSCharacter.h"

#include "AIHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "TDS/FuncLibrary/MovementTypes.h"
#include "Kismet/KismetMathLibrary.h"

ATDSCharacter::ATDSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Stamina = MaxStamina;
}

void ATDSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, TargetCameraHeight, DeltaSeconds, CameraHightStepChange);
	CameraBoom->TargetArmLength = InterpolatedZoom;


	ChangeStamina(DeltaSeconds);
}

void ATDSCharacter::CharacterUpdate()
{
	float ResSpeed = 600.f;
	switch (MovementState) {
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeed;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeed;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunSpeed;
		break;
	case EMovementState::Aim_Run_State:
		ResSpeed = MovementInfo.AimWithRunSpeed;
		break;
	case EMovementState::Sprint_State:
		ResSpeed = MovementInfo.SprintSpeed;
		break;
	default: ;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATDSCharacter::ChangeMovementState()
{
	if(!WalkEnabled && !SprintEnabled && !AimEnabled)
	{
		MovementState = EMovementState::Run_State;
	}else
	{

		if (SprintEnabled)
		{
			//WalkEnabled = false;
			AimEnabled = false;
			MovementState = EMovementState::Sprint_State;

		}
		else if (WalkEnabled && !SprintEnabled && !AimEnabled)
		{
			MovementState = EMovementState::Walk_State;

		}
		else if (WalkEnabled && !SprintEnabled && AimEnabled)
		{
			MovementState = EMovementState::Aim_State;
		}
		else if(!WalkEnabled && !SprintEnabled && AimEnabled)
		{
			MovementState = EMovementState::Aim_Run_State;
		}

	}
	CharacterUpdate();
}

void ATDSCharacter::CameraSlide(float Value)
{
	float hightResult;
	float step = CameraHightStepChange;

	if (Value < 0)
	{
		step *= -1;
	}

	hightResult = FMath::Clamp(step + TargetCameraHeight, CameraHeightMin, CameraHeightMax);

	TargetCameraHeight = hightResult;
}

void ATDSCharacter::ChangeStamina(const float Delta)
{
	float changeSpeed = Delta * 60;

	changeSpeed *= SprintEnabled ? -1 : 1;

	float changedStamina = Stamina + changeSpeed;

	if (changedStamina <= 0 || changedStamina >= MaxStamina)
	{

		Stamina = SprintEnabled ? 0 : MaxStamina;
	}
	else {
		Stamina = changedStamina;
	}
}
