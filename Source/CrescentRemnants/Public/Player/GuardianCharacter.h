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
class UCharacterMovementComponent;
class AGuardianController;

UCLASS()
class CRESCENTREMNANTS_API AGuardianCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Capsule Collider Component - the player collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> GuardianCapsuleComponent;
	
	/** Skeletal Mesh Component - the animated SkeletalMesh asset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> GuardianMeshComponent;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	/** Custom Guardian Controller (handles Input) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller", meta = (AllowPrivateAccess = "true"))
	AGuardianController* GuardianController;

	// Updated when the player is in contact with a Checkpoint.
	FVector CurrentRespawnLocation = FVector(0,0,0);  

	/** Capsule Player Collider Radius Length */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	float PlayerCapsuleColliderRadius = 48.0f;

	/** Capsule Player Collider Height */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	float PlayerCapsuleColliderHalfHeight = 96.0f;

	/** Camera Distance to the Player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraDistanceToPlayer = 300.0f;
	
	/** GuardianLook Sensitivity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float LookSensitivity = 1.0f; // How much to multiply the look input with

	/** Guardian MoveSpeed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float GuardianWalkSpeed = 400.0f;

	/** Guardian current speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float GuardianCurrentMoveSpeed = 400.0f;

	/** Guardian RunSpeed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float GuardianRunSpeed = 800.0f;
	
	/** Guardian JumpStrength */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float GuardianJumpStrength = 500.0f;

	/** Debug bool - Enable to turn on Debugging tools for the GuardianCharacter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	bool bDebugEnabled = false;

	// Our own custom Tick (instead of running 60 frames a second, we do 20 frames a second)
	void Tick20Frames() const;
	
public:

	// The constructor - should always be visible overall for Unreal and all (UPROPERTY and stuff).
	AGuardianCharacter();

	// For debugging purposes is bDebugEnabled = true;
	void DebugDraw() const;
	
	// --- Player Input Action functions ---
	
	// Called for movement input 
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianMove(const FInputActionValue& Value);

	// Called for running input 
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianRun(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianStopRun(const FInputActionValue& Value);
	
	// Called for jumping input 
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianJump(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianStopJump(const FInputActionValue& Value);
	
	// Called for camera input 
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianLook(const FInputActionValue& Value);

	// Called for when the player dies
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GuardianDeath(AActor* OtherActor);

protected:

	// Derived from AActor, whenever the player overlaps with an actor
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	// Called when the game starts or when spawned - after all scripts are initialised.
	virtual void BeginPlay() override;

	// Called when switching from e.g. keyboard to controller input, or if you can play a different controller.
	virtual void NotifyControllerChanged() override;

public:

	// --- Getters --- if other scripts need access to this. For the sake of good code practice, I have left them here.

	/** Returns Capsule Collider subObject **/
	FORCEINLINE TObjectPtr<UCapsuleComponent> GetGuardianCapsuleComponent() const { return GuardianCapsuleComponent; }
	/** Returns SkeletalMesh subObject **/
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetGuardianMeshComponent() const { return GuardianMeshComponent; }
	/** Returns CameraBoom subObject **/
	FORCEINLINE TObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subObject **/
	FORCEINLINE TObjectPtr<UCameraComponent> GetPlayerFollowCamera() const { return FollowCamera; }
	
};