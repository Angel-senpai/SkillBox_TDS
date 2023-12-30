// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "TDS/Character/TDSCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "TDS/Character/TDSCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/InputComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ATDSPlayerController::ATDSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ATDSPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	playerCharacter = Cast<ATDSCharacter>(GetCharacter());

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ATDSPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LookAtMousePosition(DeltaSeconds);
}

void ATDSPlayerController::LookAtMousePosition(float DeltaTime)
{
	// Declaration of variables to hold mouse vectors.
	FVector MouseDir = FVector::ZeroVector;
	FVector MousePos = FVector::ZeroVector;
	// Pass by reference to get mouse position in world space and direction vector.
	DeprojectMousePositionToWorld(MousePos, MouseDir);

	// Declaration of vector of intersection.
	Intersection = FVector::ZeroVector;
	float t = 0.f;
	// Vector from camera that crosses the plane we want the intersection.
	FVector LineEnd = MousePos + MouseDir * 2000.f;
	// Get intersection vector. Returns true if intersection was possible.

	const auto pawn = GetPawn();
	if (!pawn){return;}

	bool bIntersectionSuccess = UKismetMathLibrary::LinePlaneIntersection_OriginNormal(
		MousePos,
		LineEnd,
		pawn->GetActorLocation(),
		pawn->GetActorUpVector(),
		t,
		Intersection
	);
	// Do stuff if line intersected.
	if (bIntersectionSuccess)
	{
		
		pawn->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(pawn->GetActorLocation(), Intersection));
		// Debug
		DrawDebugLine(GetWorld(), pawn->GetActorLocation(), Intersection, FColor::Orange, false, -1.f, 0, 4.f);
		DrawDebugSphere(GetWorld(), Intersection, 10.f, 16, FColor::Red, false);
	}
}


void ATDSPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(SetMouseClickAction, ETriggerEvent::Triggered, this, &ATDSPlayerController::onAimTriggered);
		EnhancedInputComponent->BindAction(SetMouseClickAction, ETriggerEvent::Completed, this, &ATDSPlayerController::onAimReleased);

		EnhancedInputComponent->BindAction(SetRunAction, ETriggerEvent::Triggered, this, &ATDSPlayerController::onRunTriggered);
		EnhancedInputComponent->BindAction(SetRunAction, ETriggerEvent::Completed, this, &ATDSPlayerController::onRunReleased);

		EnhancedInputComponent->BindAction(SetWalkRunSwitcherAction, ETriggerEvent::Triggered, this, &ATDSPlayerController::onSwitchRunWalkStateTriggered);
		EnhancedInputComponent->BindAction(SetWalkRunSwitcherAction, ETriggerEvent::Completed, this, &ATDSPlayerController::onSwitchRunWalkStateReleased);
		EnhancedInputComponent->BindAction(SetMouseWheelAction, ETriggerEvent::Triggered, this, &ATDSPlayerController::inputMouseWheel);
		EnhancedInputComponent->BindAction(SetMovementAction, ETriggerEvent::Triggered, this, &ATDSPlayerController::onMovementInput);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}





void ATDSPlayerController::onAimTriggered()
{
	if (playerCharacter)
	{
		playerCharacter->AimEnabled = true;
		playerCharacter->ChangeMovementState();
	}
}

void ATDSPlayerController::onAimReleased()
{
	if (playerCharacter)
	{
		playerCharacter->AimEnabled = false;
		playerCharacter->ChangeMovementState();
	}
}

void ATDSPlayerController::onRunTriggered()
{
	if (playerCharacter)
	{
		auto rotation = playerCharacter->GetActorRotation();
		auto velocity = playerCharacter->GetVelocity();

		auto delta = UKismetMathLibrary::NormalizedDeltaRotator(rotation, velocity.Rotation());

		if (UKismetMathLibrary::Abs(delta.Yaw) <= 20 && playerCharacter->Stamina != 0)
		{
			playerCharacter->SprintEnabled = true;
			GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Red, TEXT("Sprint on"));
		}else
		{
			playerCharacter->SprintEnabled = false;
			GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Red, TEXT("Sprint off"));
			GEngine->AddOnScreenDebugMessage(1555, 0.5f, FColor::Green, FString::Printf(TEXT("%f"), playerCharacter->Stamina));
		}
		playerCharacter->ChangeMovementState();
	}
}

void ATDSPlayerController::onRunReleased()
{
	if (playerCharacter)
	{
		playerCharacter->SprintEnabled = false;
		playerCharacter->ChangeMovementState();

		GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Red, TEXT("Sprint off"));
	}
}

void ATDSPlayerController::onSwitchRunWalkStateTriggered()
{
	if (playerCharacter)
	{
		playerCharacter->WalkEnabled = true;
		playerCharacter->ChangeMovementState();
	}
}

void ATDSPlayerController::onSwitchRunWalkStateReleased()
{
	if (playerCharacter)
	{
		playerCharacter->WalkEnabled = false;
		playerCharacter->ChangeMovementState();
	}
}

void ATDSPlayerController::inputMouseWheel(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	if (playerCharacter)
	{
		playerCharacter->CameraSlide(DirectionValue);
	}

}

void ATDSPlayerController::onMovementInput(const FInputActionValue& Value)
{
	const FVector2D Vector = Value.Get<FVector2D>();
	if (playerCharacter)
	{
		

		const auto controlRotation = playerCharacter->GetControlRotation();
		const auto rightVector = UKismetMathLibrary::GetRightVector(FRotator(0, controlRotation.Yaw, controlRotation.Roll));
		playerCharacter->AddMovementInput(rightVector, Vector.X);

		const auto forwardVector = UKismetMathLibrary::GetForwardVector(FRotator(0, controlRotation.Yaw, 0));


		playerCharacter->AddMovementInput(forwardVector, Vector.Y);


	}
}


