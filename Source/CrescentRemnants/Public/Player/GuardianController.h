#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Player/GuardianCharacter.h"
#include "GuardianController.generated.h"

/**
 * 
 */
UCLASS()
class CRESCENTREMNANTS_API AGuardianController : public APlayerController
{
	GENERATED_BODY()

	/** InputMappingContext (IMC) for the Guardian */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* IMC_Guardian;
	
	virtual void SetupInputComponent() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	
	void BindInputs(AGuardianCharacter* GuardianCharacter, UEnhancedInputComponent* EnhancedInputComponent);

protected:

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

	/**
		 * FORCEINLINE forces the compiler to inline the function, but it increases the size of it.
		 * Ideal for very small, frequently called getters, to reduce function call overhead. These just return a pointer.
		 * Big functions would leave copies of inlined code everywhere...
		 * For example:
		 * Normal Function: 1x 20 bytes = 20 bytes total (just one function in memory).
		 * 10 Inline Functions: 10x 20 bytes = 200 bytes total (10 separate copies!).
		*/
	
	// Getter function for the mapping context
	UFUNCTION(BlueprintCallable, Category = "Input")
	FORCEINLINE class UInputMappingContext* GetMappingContext() const { return IMC_Guardian; }
};
