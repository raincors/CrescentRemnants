// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CrescentRemnantsGameMode.generated.h"

UCLASS(minimalapi)
class ACrescentRemnantsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	ACrescentRemnantsGameMode();
	

protected:
	
	// This is called before anything is spawned, and when a level is loaded for the first time. 
	//virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UPROPERTY(EditDefaultsOnly, Category = "Guardian")
	TSubclassOf<class AGuardianCharacter> GuardianCharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Guardian")
	TSubclassOf<class AGuardianController> GuardianControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Guardian")
	TSubclassOf<class ARemnantHUD> GuardianHUDClass;
};



