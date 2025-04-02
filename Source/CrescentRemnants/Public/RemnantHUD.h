// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/ProgressBar.h"
#include "RemnantHUD.generated.h"

class URemnantsProgressBar;
/**
 * 
 */
UCLASS()
class CRESCENTREMNANTS_API ARemnantHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category=Remnants)
	TSubclassOf<URemnantsProgressBar> RemnantsProgressBarClass;

	UPROPERTY()
	URemnantsProgressBar* RemnantsProgressBar;

public:
	FORCEINLINE URemnantsProgressBar* GetRemnantsProgressBar() const { return RemnantsProgressBar; }
};
