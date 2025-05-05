// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "InputActionValue.h"
#include "GuardianCharacter.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterMovementComponent;
class APickup;
class UTextBubble;
class AGuardianController;

struct FInputActionValue;

/**
 * UCLASS(Config=Game) means that default values are being stored in DefaultGame.ini, letting us tweak settings directly
 * in the .ini files. It means we can change the values in .ini without having to recompile the game every time.
 * 
 * "Config=" doesn't always have to lead to "Game", it can lead to =Engine, =Editor, =Input, etc.
 * Config only loads once at start-up though, so don't use it with variables that need to change in run-time.
 *
 * For dynamic values, use USTRUCT: e.g. USTRUCT(BlueprintType) struct FPlayerStats { GENERATED_BODY() ... }
 * and store it in your character class: UPROPERTY(EditAnywhere, BlueprintReadWrite, ...) FPlayerStats PlayerStats;
 * If you need dynamic values saved: use UCharacterSaveGame : public USaveGame, with functions ::SaveGame and ::LoadGame
 * 
 * Properties that should always be visible in Blueprints, should utilize UPROPERTY and "EditAnywhere" instead.
 * But... for C++ only coders, Config= and .ini files are great too. :)
*/
UCLASS(Config=Game)
class CRESCENTREMNANTS_API AGuardianCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Capsule Collider Component - the player collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> GuardianCapsuleComponent;
	
	/** Skeletal Mesh Component - the animated SkeletalMesh asset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> GuardianMeshComponent;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	/** Sphere Collision (Pick-up Radius) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> PickupRadiusSphere;

	/** Custom Guardian Controller (handles Input) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller", meta = (AllowPrivateAccess = "true"))
	AGuardianController* GuardianController;

	/** Capsule Player Collider Radius Length */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float PlayerCapsuleColliderRadius = 42.0f;

	/** Capsule Player Collider Height */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float PlayerCapsuleColliderHalfHeight = 96.0f;

	/** Camera Distance to the Player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float CameraDistanceToPlayer = 350.0f;

	/** Guardian MoveSpeed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float GuardianMoveSpeed = 500.0f;

	/** Guardian current speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float GuardianCurrentSpeed = 500.0f;

	/** Guardian RunSpeed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float GuardianRunSpeed = 800.0f;
	
	/** Guardian JumpStrength */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float GuardianJumpStrength = 500.0f;
	
	/**  The radius in which the player can interact/pick up items */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float PickupRadiusLength = 45.0f;

	/** GuardianInteract Hold Threshold; how long GuardianInteract must be held before long GuardianInteract happens */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", meta = (AllowPrivateAccess = "true"))
	float InteractHoldTimeThreshold = 1.0f; // How long it takes for LongInteract to happen

	/** GuardianLook Sensitivity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", meta = (AllowPrivateAccess = "true"))
	float LookSensitivity = 1.0f; // How much to multiply the look input with

	/** Debug bool - Enable to turn on Debugging tools for the GuardianCharacter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging", meta = (AllowPrivateAccess = "true"))
	bool bDebugEnabled = false;
	
	/** Is the Guardian climbing? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", meta = (AllowPrivateAccess = "true"))
	bool bIsClimbing = false; // Tracks if the Guardian is climbing.

	//Remnants pick-up variables:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remnants", meta = (AllowPrivateAccess = "true"))
	float RemnantsProgress = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remnants", meta = (AllowPrivateAccess = "true"))
	int Memory = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remnants", meta = (AllowPrivateAccess = "true"))
	float RemnantsCounter = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remnants", meta = (AllowPrivateAccess = "true"))
	float MaxRemnants = 3.0f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTextBubble> TextBubbleClass;

	UPROPERTY()
	UTextBubble* TextBubble;
	// Remnant pick-ups variables ^^^
	
	FTimerHandle InteractHoldTimerHandle; // Timer Handle to help track hold duration with GuardianInteract
	float StoredInteractHoldTime = 0.0f; // Stores how long the GuardianInteract button is held;
	
	FTimerHandle PickupTimerHandle; // Timer Handle for often the player checks for nearby pick-ups.
	float PickupCheckTimeInterval = 0.10f; // The actual time between checks in float value.
	
public:
	AGuardianCharacter();

	void DebugDraw() const;

	FVector CurrentRespawnLocation = FVector(0,0,0);  // Updated when player is in contact with a Checkpoint.

	// Player Input Action functions - functions called when the player does any kind of input on the controller:
	
	/** Called for movement input */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianMove(const FInputActionValue& Value);

	/** Called for running input */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianRun(const FInputActionValue& Value);
	void GuardianStopRun(const FInputActionValue& Value);
	
	/** Called for jumping input */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianJump(const FInputActionValue& Value);
	void GuardianStopJumping(const FInputActionValue& Value);
	virtual void Landed(const FHitResult& Hit) override;
	
	/** Called for camera input */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianLook(const FInputActionValue& Value);

	/** Called for interact input */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianInteract();

	/** Called for when player dies */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianDeath(AActor* OtherActor);

	/** Called for escape input (pause screen, skip etc.) */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianEscape(const FInputActionValue& InputActionValue);

	/** Internal character functions below (not directly connected to input from the controller): */

	// Derived from AActor, whenever the player overlaps with an actor
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UFUNCTION(BlueprintCallable, Category = "Character")
	void CheckForNearbyPickups();

	/** Called for GuardianInteract input */
	void StartInteract(const FInputActionValue& Value);

	/** Called when GuardianInteract input ends */
	void StopInteract(const FInputActionValue& Value);

	bool IsOnGround() const;
	FVector GetFloorNormal() const;

protected:

	/** Called when Interacting; has to do with short or long interact */
	void UpdateHoldTime();
	void PerformShortInteract();
	void PerformLongInteract();


	// Called when the game starts or when spawned - after all scripts are initialized.
	virtual void BeginPlay() override;

	// Called when switching from e.g. keyboard to controller input, or if you can play a different controller.
	virtual void NotifyControllerChanged() override;
	
	// Called during initialization of script, before BeginPlay() to set-up playerInput.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Running 60 frames a second.
	virtual void Tick(float DeltaTime) override;


public:	

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool GetIsClimbing() const { return bIsClimbing; }

	/** Returns Capsule Collider subObject **/
	FORCEINLINE TObjectPtr<UCapsuleComponent> GetGuardianCapsuleComponent() const { return GuardianCapsuleComponent; }
	/** Returns SkeletalMesh subObject **/
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetGuardianMesh() const { return GuardianMeshComponent; }
	/** Returns CameraBoom subObject **/
	FORCEINLINE TObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subObject **/
	FORCEINLINE TObjectPtr<UCameraComponent> GetFollowCamera() const { return FollowCamera; }
	/** Returns PickupSphere subObject **/
	FORCEINLINE TObjectPtr<USphereComponent> GetPickupSphereComponent() const { return PickupRadiusSphere; }
	
	UFUNCTION(BlueprintCallable)
	void RemnantCollect(APickup* Pickup);
	
	UFUNCTION(BlueprintCallable)
	void MemoryUnlock();

	UFUNCTION(BlueprintCallable)
	void ResetRemnantProgress();

private:

	//class and function for the enemyAI's perception system
	class UAIPerceptionStimuliSourceComponent* StimulusSource;
	void SetupStimulusSource();
};

/** Functions and variables intended for jumping (deriving only what's necessary from ACharacter):
	*	public:
	*		void GuardianJump() - to be able to jump
	*		void StopJumping() - to prevent receiving vertical velocity more than allowed, and reset JumpCounter etc.
	*			Should be called when jumpInput is stopped.
	*		bool bCanJump const - returns if the character can jump right now (maybe do same for Climbing?)
	*		void isFalling() - to know when the character is mid-air.
	*
	*		UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Character) for all the below:
	*		int32 JumpsMaxCount - Counter to keep track of max jumps available.
	*		int32 JumpCurrentCount - Counter to keep track of current jumps performed - reset in StopJumping().
*/