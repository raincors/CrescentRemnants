// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy.h"
#include "Player/GuardianCharacter.h"

// Sets default values
AAIEnemy::AAIEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAIEnemy::NotifyActorBeginOverlap(AActor* OtherActor)
{
	
}