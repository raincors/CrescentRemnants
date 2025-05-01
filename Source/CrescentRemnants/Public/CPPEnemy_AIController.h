// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "CPPEnemy_AIController.generated.h"

/**
 * 
 */
UCLASS()
class CRESCENTREMNANTS_API ACPPEnemy_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	explicit ACPPEnemy_AIController(FObjectInitializer const& ObjectInitializer);

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	class UAISenseConfig_Sight* SightConfig;
	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
