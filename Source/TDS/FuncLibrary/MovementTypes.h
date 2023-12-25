// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MovementTypes.generated.h"


UENUM(BlueprintType)
enum class EMovementState: uint8
{
	Aim_State	  UMETA(DisplayName = "Aim State"),
	Walk_State    UMETA(DisplayName = "Walk State"),
	Run_State	  UMETA(DisplayName = "Run State"),
	Aim_Run_State UMETA(DisplayName = "Aim Run State"),
	Sprint_State  UMETA(DisplayName = "Sprint State"),

};

USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float AimSpeed = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float WalkSpeed = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float RunSpeed = 450.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float AimWithRunSpeed = 350.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float SprintSpeed = 600.f;
	
};

UCLASS()
class TDS_API UTypes: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
