#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Player/GuardianCharacter.h"
#include "GuardianController.generated.h"

class UInputMappingContext;

/**
 * AGuardianController is a custom player controller class designed for controlling the Guardian character in the game.
 * This class sets up an enhanced input system for player interaction, defines input mappings, and manages the player's possession behaviour.
 */
UCLASS()
class CRESCENTREMNANTS_API AGuardianController : public APlayerController
{
	GENERATED_BODY()
	
	/** InputMappingContext (IMC) for the Guardian */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* IMC_Guardian;

	/** GuardianMove Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Move;

	/** GuardianRun Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Run;

	/** GuardianJump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Jump;

	/** GuardianInteract Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Interact;

	/** GuardianLook Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Look;

	/** GuardianEscape Input Action - Pause and Unpause */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Escape;
	
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	void BindInputs(AGuardianCharacter* GuardianCharacter, UEnhancedInputComponent* EnhancedInputComponent);

public:

	// Getter methods to access IA_* actions
	UInputAction* GetIA_Move() const { return IA_Move; }
	UInputAction* GetIA_Run() const { return IA_Run; }
	UInputAction* GetIA_Jump() const { return IA_Jump; }
	UInputAction* GetIA_Interact() const { return IA_Interact; }
	UInputAction* GetIA_Look() const { return IA_Look; }
	UInputAction* GetIA_Escape() const { return IA_Escape; }

	UFUNCTION(BlueprintCallable, Category = "Input")
	UEnhancedInputComponent* GetEnhancedInputComponent() const { return Cast<UEnhancedInputComponent>(InputComponent); }
	
	// Getter function for the mapping context
	UFUNCTION(BlueprintCallable, Category = "Input")
	FORCEINLINE class UInputMappingContext* GetMappingContext() const { return IMC_Guardian; }
};
