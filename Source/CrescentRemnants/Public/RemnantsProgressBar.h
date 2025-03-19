// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RemnantsProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class CRESCENTREMNANTS_API URemnantsProgressBar : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetPercent(float Percent);

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* RemnProgressBar;
	
};
