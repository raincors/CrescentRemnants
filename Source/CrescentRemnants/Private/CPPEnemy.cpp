// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPEnemy.h"

// Sets default values
ACPPEnemy::ACPPEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACPPEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPPEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACPPEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UBehaviorTree* ACPPEnemy::GetBehaviorTree() const
{
	return Tree;
}