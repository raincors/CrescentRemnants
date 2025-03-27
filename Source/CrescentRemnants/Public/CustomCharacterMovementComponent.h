// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CRESCENTREMNANTS_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Function to detect ledges (derived from UCharacterMovementComponent)
	UFUNCTION(BlueprintCallable, Category = "Ledge Climbing")
	bool DetectLedge(FVector& OutLedgePosition);

	// Variable to determine how far ahead the player detects ledges
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ledge Climbing", Meta = (AllowPrivateAccess))
	float ForwardVectorLedgeDetectRange = 100.f;
	
};
