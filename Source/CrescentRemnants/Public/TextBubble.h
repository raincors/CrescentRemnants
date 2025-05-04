// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextBubble.generated.h"

class AGuardianCharacter;
/**
 * 
 */
UCLASS()
class CRESCENTREMNANTS_API UTextBubble : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Blueprintable)
	FString GetMemory(int Memory);
	
	
};
