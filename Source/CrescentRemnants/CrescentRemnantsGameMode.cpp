// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrescentRemnantsGameMode.h"
#include "Player/GuardianCharacter.h"
#include "Player/GuardianController.h"
#include "RemnantHUD.h"

ACrescentRemnantsGameMode::ACrescentRemnantsGameMode()
{
	// Load the GuardianCharacter Blueprint - keep in mind of the extra _C at the end (use the generated class)! 
	static ConstructorHelpers::FClassFinder<AGuardianCharacter> CharBPClass(TEXT("/Game/Blueprints/BP_GuardianCharacter.BP_GuardianCharacter_C"));
	if (CharBPClass.Succeeded())
	{
		GuardianCharacterClass = CharBPClass.Class;
		DefaultPawnClass = GuardianCharacterClass;
	}

	// Load the custom GuardianController (C++)
	PlayerControllerClass = AGuardianController::StaticClass();

	// Load the custom HUD Blueprint -  - keep in mind of the extra _C at the end (use the generated class)! 
	static ConstructorHelpers::FClassFinder<ARemnantHUD> HUDBPClass(TEXT("/Game/Blueprints/HUD/BP_HUD.BP_HUD_C"));
	if (HUDBPClass.Succeeded())
	{
		GuardianHUDClass = HUDBPClass.Class;
		HUDClass = GuardianHUDClass;
	}
}
