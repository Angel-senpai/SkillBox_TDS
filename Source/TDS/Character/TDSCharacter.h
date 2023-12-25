// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDS/FuncLibrary/MovementTypes.h"
#include "TDSCharacter.generated.h"

enum class EMovementState: uint8;
struct FCharacterSpeed;

UCLASS(Blueprintable)
class ATDSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATDSCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Run_State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FCharacterSpeed MovementInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		bool SprintEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool WalkEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool AimEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float TargetCameraHeight = 1500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float CameraHeightMax = 2000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float CameraHeightMin = 700;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float CameraHightStepChange = 100;

	UFUNCTION(BlueprintCallable)
		void CharacterUpdate();
	UFUNCTION(BlueprintCallable)
		void ChangeMovementState();

	UFUNCTION(BlueprintCallable)
		void CameraSlide(float Value);

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
};

