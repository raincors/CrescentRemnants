// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"
#include "GameplayStatics.generated.h"
#include "CrescentRemnantsCharacter.generated.h"

// Sets default values
AEnemyAI::AEnemyAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AEnemyAI::GetPlayerPos()
{
	
	return
}