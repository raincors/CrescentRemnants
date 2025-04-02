// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrescentRemnantsGameMode.h"
#include "Player/GuardianCharacter.h"
#include "Player/GuardianController.h"

ACrescentRemnantsGameMode::ACrescentRemnantsGameMode()
{
    // Set the default pawn class to my pure C++ character Guardian
		DefaultPawnClass = AGuardianCharacter::StaticClass();

	// Set the default HUD class to be our HUD class:
		// HUDClass = AGuardianHUDClass::StaticClass();
    
	// Set the default controller class to my own C++ controller for the enhanced input system
		PlayerControllerClass = AGuardianController::StaticClass();

	// Set the default Game State class to be:
		// GameStateClass = AGameStateClassHere::StaticClass();
    
	/*
		// --- Blueprint versions:
		// If I were to use a Blueprint class, I'd have to load it like this inside the constructor:
    	
    // Finding and locating the exact blueprint (be precise with syntax around the file path)
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Benjamin/BP_Protagonist"));
    	
    if (PlayerPawnBPClass.Succeeded())
    {
    	DefaultPawnClass = PlayerPawnBPClass.Class;
    }
    */
}
void ACrescentRemnantsGameMode::StartPlay()
{
	Super::StartPlay();

	// Just for testing purposes, and to see how to debug to the screen - 
	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("Hello World, this is from CrescentRemnantsGameMode.cpp!"));
}
