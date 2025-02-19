// Copyright Epic Games, Inc. All Rights Reserved.

#include "AwakeningGameMode.h"
#include "AwakeningCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAwakeningGameMode::AAwakeningGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
