// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/GuardianController.h"
#include "Player/GuardianCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

/** Order of assignments when it comes to input actions, mapping context and subsystems and all that:
 *
 *	AGuardianController::SetupInputComponent
 * 1. Make sure the EnhancedInputComponent exists, otherwise all input bindings have nothing to bind to.
 * 2. Load the Input Mapping Context (IMC) - without this "profile", the input bindings still have nothing to bind to.
 * 3. Load the Input Actions and check that we can bind them.
 * 4. Get the EnhancedInputLocalPlayerSubsystem, to ensure the IMC is applied to the right player in the game.
 *
 *	AGuardianController::OnPossess
 * 5. Applying the mapping context, with bindings to the subsystem (activating the controls).
 * 6. Get the GuardianCharacter pawn to assign the input bindings to.
 * 7. AGuardianController::BindInputs (now that all is loaded and checked).
 */
void AGuardianController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 1. Get the Enhanced Input Component
	UEnhancedInputComponent* EnhancedInputComponent = GetEnhancedInputComponent();
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ EnhancedInputComponent not found! Input bindings will fail."));
	}
	
	// 2. Load Input Mapping Context if it is not already set
	if (!IMC_Guardian)
	{
		IMC_Guardian = Cast<UInputMappingContext>(StaticLoadObject(UInputMappingContext::StaticClass(), nullptr, TEXT("InputMappingContext'/Game/Input/IMC_Guardian.IMC_Guardian'")));
		if (IMC_Guardian)
		{
			UE_LOG(LogTemp, Warning, TEXT("💂 Successfully loaded %s"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to load InputMappingContext - IMC_Guardian"));
		}
	}

	// 3. Load all respective Input Actions - I also check every IA_* due to strictly typing file paths with loading 'em.
	if (!IA_Move)
	{
		IA_Move = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Move.IA_Move'")));
		if (IA_Move) { UE_LOG(LogTemp, Warning, TEXT("💂 Successfully loaded IA_Move")); }
		else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Move!")); }
	}
	if (!IA_Run)
	{
		IA_Run = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Run.IA_Run'")));
		if (IA_Run) { UE_LOG(LogTemp, Warning, TEXT("💂 Successfully loaded IA_Run")); }
		else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Run!")); }
	}
	if (!IA_Jump)
	{
		IA_Jump = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Jump.IA_Jump'")));
		if (IA_Jump) { UE_LOG(LogTemp, Warning, TEXT("💂 Successfully loaded IA_Jump")); }
		else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Jump!")); }
	}
	if (!IA_Interact)
	{
		IA_Interact = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Interact.IA_Interact'")));
		if (IA_Interact) { UE_LOG(LogTemp, Warning, TEXT("💂 Successfully loaded IA_Interact")); }
		else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Interact!")); }
	}
	if (!IA_Look)
	{
		IA_Look = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Look.IA_Look'")));
		if (IA_Look) { UE_LOG(LogTemp, Warning, TEXT("💂 Successfully loaded IA_Look")); }
		else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Look!")); }
	}
	if (!IA_Escape)
	{
		IA_Escape = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Escape.IA_Escape'")));
		if (IA_Escape) { UE_LOG(LogTemp, Warning, TEXT("💂 Successfully loaded IA_Escape")); }
		else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Escape!")); }
	}
	
	// 4. Get the Local Player Subsystem - which manages input mappings per local player.
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!InputSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to get UEnhancedInputLocalPlayerSubsystem! Input mapping will not be applied."));
		return;
	}

	// 5. Apply the Input Mapping Context to the Local Player Subsystem
	if (IMC_Guardian)
	{
		UE_LOG(LogTemp, Warning, TEXT("💂 Mapping Context Added to the Local Subsystem!"));
		InputSubsystem->AddMappingContext(IMC_Guardian, 0);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Mapping Context NOT Added inside GuardianController.cpp!"));
	}
}

void AGuardianController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 6. Get the controlled GuardianCharacter - Ensure we bind all this to the right pawn.
	if (AGuardianCharacter* GuardianCharacter = Cast<AGuardianCharacter>(InPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("💂 GuardianController now possesses: %s"), *GuardianCharacter->GetName());

		// 7. Now that we have a valid character, bind inputs.
		BindInputs(GuardianCharacter, GetEnhancedInputComponent());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ OnPossess: GuardianCharacter is NULL!"));
	}
}

// Binding loaded input actions with the EnhancedInputComponent to the chosen playerCharacter
void AGuardianController::BindInputs(AGuardianCharacter* GuardianCharacter, UEnhancedInputComponent* EnhancedInputComponent)
{
	// Bind GuardianMove action
	if (GetIA_Move())
	{
		EnhancedInputComponent->BindAction(GetIA_Move(), ETriggerEvent::Triggered, GuardianCharacter, &AGuardianCharacter::GuardianMove);
	}
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Move is NULL!")); }

	// Bind GuardianRun action
	if (GetIA_Run())
	{
		EnhancedInputComponent->BindAction(GetIA_Run(), ETriggerEvent::Triggered, GuardianCharacter, &AGuardianCharacter::GuardianRun);
		EnhancedInputComponent->BindAction(GetIA_Run(), ETriggerEvent::Completed, GuardianCharacter, &AGuardianCharacter::GuardianRun);
	}
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Run is NULL!")); }

	// Bind GuardianJump actions
	if (GetIA_Jump())
	{
		EnhancedInputComponent->BindAction(GetIA_Jump(), ETriggerEvent::Started, GuardianCharacter, &AGuardianCharacter::GuardianJump);
		EnhancedInputComponent->BindAction(GetIA_Jump(), ETriggerEvent::Completed, GuardianCharacter, &AGuardianCharacter::GuardianStopJumping);
	}
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Jump is NULL!")); }

	// Bind GuardianInteract action
	if (GetIA_Interact())
	{
		EnhancedInputComponent->BindAction(GetIA_Interact(), ETriggerEvent::Started, GuardianCharacter, &AGuardianCharacter::StartInteract);
		EnhancedInputComponent->BindAction(GetIA_Interact(), ETriggerEvent::Completed, GuardianCharacter, &AGuardianCharacter::StopInteract);
	}
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Interact is NULL!")); }

	// Bind GuardianLook action
	if (GetIA_Look())
	{
		EnhancedInputComponent->BindAction(GetIA_Look(), ETriggerEvent::Triggered, GuardianCharacter, &AGuardianCharacter::GuardianLook);
	}
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Look is NULL!")); }

	// Bind GuardianEscape action
	if (GetIA_Escape())
	{
		EnhancedInputComponent->BindAction(GetIA_Escape(), ETriggerEvent::Started, GuardianCharacter, &AGuardianCharacter::GuardianEscape);
	}
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Escape is NULL!")); }
}
