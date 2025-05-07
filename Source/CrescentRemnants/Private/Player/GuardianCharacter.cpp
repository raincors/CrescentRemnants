// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/GuardianCharacter.h"
#include "Player/GuardianController.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Player/PlayerCheckpoint.h"
#include "Pickup.h"
#include "Blueprint/UserWidget.h"
#include "TextBubble.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

/**
	* Overview and Execution Order of the code:
	* 1. Constructor AGuardianCharacter() - When Actor is instantiated (before Play mode).
	* 2. NotifyControllerChanged() - When the character gets a controller. (And when you exit Playmode)
	* 3. SetupPlayerInputComponent() - Runs when Unreal sets up input for the character.
	* 4. BeginPlay() - Character is now fully in the world, and Play Mode is started.
	* 
	* 5. Tick20Frames() - Cheaper Tick() function, and runs 20 times per second.
	*
	* GuardianMove(), GuardianInteract(), etc. - Only runs when buttons are pressed.
 */

// Sets default values
AGuardianCharacter::AGuardianCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Mostly to ensure the player always has a place to spawn, if dying, and the player has not touched a checkpoint.
	CurrentRespawnLocation = GetActorLocation();
	
	// Setting up our collision capsule from CharacterMovementComponent, with the right desired properties
	GuardianCapsuleComponent = GetCapsuleComponent();
	GuardianCapsuleComponent->InitCapsuleSize(PlayerCapsuleColliderRadius, PlayerCapsuleColliderHalfHeight);
	// If the below isn't done, we won't get overlap events with the capsule!
	GuardianCapsuleComponent->SetGenerateOverlapEvents(true);
	
	// Have to set a root component! IMPORTANT (...usually).
	SetRootComponent(GuardianCapsuleComponent);
	
	// Now this below collision stuff is wack, cool stuff:
	// PS. You can have custom collision channels too.
	
	// QueryOnly would do overlaps and traces, but block nothing; PhysicsOnly would be great for physics simulations,
	// NoCollision explains itself, and QueryAndPhysics gets overlaps and blocking (best for characters).
	GuardianCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// What kind of object are we? ECC_Pawn works for Characters, ECC_WorldStatic for Walls/Floor, ECC_WorldDynamic
	// for things that move or interact. Use ECollisionChannel:: to find more options.
	GuardianCapsuleComponent->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);

	// What is our default response to collisions? Block. (ECR_Ignore, ECR_Block, ECR_MAX, and ECR_Overlap etc.)
	// Block makes sure we collide with most items (avoid falling through the ground).
	GuardianCapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Except... We don't want to block pickups! They're part of the Pawn channel, and we set our responses to those channels to be ECR_Overlap.
	GuardianCapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// And for WorldDynamic, just to be safe, we're doing Block and Overlap via MAX.
	GuardianCapsuleComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_MAX);

	// We don't want to simulate physics on the capsuleComponent
	GuardianCapsuleComponent->SetSimulatePhysics(false);
	
	
	// Setting up our Guardian SkeletalMesh component (animated mesh with rig)
	GuardianMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GuardianSkeletalMesh"));
	GuardianMeshComponent->SetupAttachment(GuardianCapsuleComponent); // Attach to root
	
	// Setting the location at the Guardian's feet. Makes it exact according to capsule half-height.
	GuardianMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -PlayerCapsuleColliderHalfHeight));
	// Setting an offset in accordance with our skeletalMesh
	GuardianMeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GuardianMeshComponent->SetSimulatePhysics(false);

	// Finding and setting the right skeletalMeshAsset.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GuardianMeshAsset(TEXT("SkeletalMesh'/Game/GuardianCharacter/augh.augh'"));
    if (GuardianMeshAsset.Succeeded())
    {
	    GuardianMeshComponent->SetSkeletalMesh(GuardianMeshAsset.Object);
    }

	// Find and set the Animation Blueprint for the Guardian
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(TEXT("AnimBlueprint'/Game/GuardianCharacter/Animations/ABP_Guardian.ABP_Guardian_C'"));
	if (AnimBPClass.Succeeded())
	{
		GuardianMeshComponent->SetAnimInstanceClass(AnimBPClass.Class);
	}
	
	// Create the camera boom (spring arm) and attach it to the root
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GuardianCapsuleComponent);
	CameraBoom->TargetArmLength = CameraDistanceToPlayer; // Distance between camera and the player it follows
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Enabling camera collision avoidance - to avoid clipping through the environment
	CameraBoom->bDoCollisionTest = true;
	// Adjusting the camera sphere collision query size, the smaller the sphere, the more precise it is.
	CameraBoom->ProbeSize = 38.0f;
	// Choosing the right collision channel for the cameraBoom (static objects generally should block ECC_Camera)
	CameraBoom->ProbeChannel = ECC_Camera;

	// Enable smoothing to help adjust the camera a little bit with a delay. Optional.
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 5.0f;
	CameraBoom->CameraLagMaxDistance = 100.0f;

	// Create the follow camera and attach it to the boom's socket
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	
	// Attach the camera to the end of the boom (in the socket) and let the boom adjust to match the controller orientation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to the arm
	
	// Get-, and Set up Character Movement Component from ACharacter
	if (GetCharacterMovement())
	{
		// Ensuring right movement mode and animation is played + gravity.
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->GravityScale = 1.0f; // Default gravity scale (adjust if needed)

		// Character orientation
		GetCharacterMovement()->bOrientRotationToMovement = true; // Turns character in towards Movement
		GetCharacterMovement()->bUseControllerDesiredRotation = false; // Controller's rotation should not affect the player.

		// Setting the default value for GuardianCurrentMoveSpeed to be GuardianWalkSpeed.
		GuardianCurrentMoveSpeed = GuardianWalkSpeed;
		
		// Walking
		GetCharacterMovement()->MaxWalkSpeed = GuardianCurrentMoveSpeed; // Walking speed
		GetCharacterMovement()->MaxStepHeight = 90.f; // Max step height
		GetCharacterMovement()->SetWalkableFloorAngle(45.f); // What tall ledges can you ascend?
		GetCharacterMovement()->bCanWalkOffLedges = true; // Allow ledge climbing or walking off edges

		// Jumping
		GetCharacterMovement()->JumpZVelocity = GuardianJumpStrength; // Jumping speed
		GetCharacterMovement()->AirControl = 0.35f; // Control mid-air - should be less than 0.5f
		JumpMaxCount = 2; // We don't want more than 2 jumps.
	}

	//Registers the player with the perception system, which allows enemies to spot them
	SetupStimulusSource();
}

// For drawing debug with the GuardianCharacter as needed during testing. Only called when bDebugEnabled.
void AGuardianCharacter::DebugDraw() const
{
	// Debug movement ForwardDirection
	FVector ForwardDirection = GuardianCapsuleComponent->GetForwardVector();
	DrawDebugLine(GetWorld(),
		GuardianCapsuleComponent->GetComponentLocation(),
		GuardianCapsuleComponent->GetComponentLocation() + ForwardDirection * 200.f,
		FColor::Blue, false, -1, 0, 2.f);

	// Debug Capsule Position
	DrawDebugCapsule(GetWorld(), 
		GuardianCapsuleComponent->GetComponentLocation(), 
		GuardianCapsuleComponent->GetScaledCapsuleHalfHeight(),
		GuardianCapsuleComponent->GetScaledCapsuleRadius(),
		FQuat::Identity, 
		FColor::Red, false, -1, 0, 2.0f);

	// Debug Mesh Position
	DrawDebugBox(GetWorld(),
		GuardianMeshComponent->GetComponentLocation(),
		FVector(50,50,50), // Size
		FColor::Blue, false, -1, 0, 2.0f);
}

// Called when the game starts or when Actor is spawned
void AGuardianCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Just running a check if GuardianController isn't found.
	GuardianController = Cast<AGuardianController>(GetController());
	
	if (!GuardianController)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ GuardianController is NULL (from GuardianCharacter)!"));
	}

	CurrentRespawnLocation = GetActorLocation();
}

/** This function runs very early when you start PlayMode, and whenever a controller is changed.
 * If we want to change controllers in-game and change the displayed HUD according to the current PlayerInputDevice, then
 * this needs to be expanded with functionality to reapply mapping contexts from GuardianController.
 * This functionality will ensure the right context is sent when using a controller VS a keyboard.
*/ 
void AGuardianCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	
	// This nasty piece of log (sneaky conditional statement) actually tries to find the controller that triggers
	// NotifyControllerChanged(). If GetController returns a name, we print that name, else we print nothing.
	UE_LOG(LogTemp, Warning, TEXT("💂 NotifyControllerChanged Called! Controller: %s"), 
		   (GetController() ? *GetController()->GetName() : TEXT("None")));
}

void AGuardianCharacter::Tick20Frames() const
{
	if (bDebugEnabled)
	{
		// DebugGroundCheck();
		DebugDraw();
	}
}

void AGuardianCharacter::RemnantCollect(APickup* Pickup)
{
	if (Pickup == nullptr)
		return;
	if (RemnantsProgress<1.0)
	{
		if (Pickup->bIsPickup==true)
		{
			Pickup->Destroy();
			RemnantsCounter++;
			RemnantsProgress = RemnantsCounter/MaxRemnants;
			GEngine->AddOnScreenDebugMessage(-1,15.0f,FColor::Magenta, FString::SanitizeFloat(RemnantsProgress));
		}
	}
}

void AGuardianCharacter::MemoryUnlock()
{
	
	if (RemnantsProgress>=1.0)
	{
		Memory++;
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC && TextBubbleClass)
		{
			TextBubble=CreateWidget<UTextBubble>(PC, TextBubbleClass);
			TextBubble->AddToViewport();
		}

	}
}

void AGuardianCharacter::ResetRemnantProgress()
{
	if (RemnantsProgress>=1.0)
	{
		RemnantsCounter = 0.0f;
		RemnantsProgress = 0.0f;
	}
}

void AGuardianCharacter::GuardianMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	// If movementInput is received, but the player isn't moving, let's not run the code.
	if (MovementVector.IsZero()) return; 
	
	// Get controller rotation to determine the direction the character is facing
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Get forward and right movement directions based on yaw rotation
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	FVector MoveDirection = (MovementVector.X * RightDirection + MovementVector.Y * ForwardDirection).GetSafeNormal();

	// Apply movement velocity directly to the CharacterMovement component
	GetCharacterMovement()->AddInputVector(MoveDirection);
}

// We assign GuardianRunSpeed to GuardianCurrentMoveSpeed, and MaxWalkSpeed in the CharacterMovementComponent. 
void AGuardianCharacter::GuardianRun(const FInputActionValue& Value)
{
	GuardianCurrentMoveSpeed = GuardianRunSpeed;
	GetCharacterMovement()->MaxWalkSpeed = GuardianCurrentMoveSpeed;
}

// We assign GuardianWalkSpeed to GuardianCurrentMoveSpeed, and MaxWalkSpeed in the CharacterMovementComponent. 
void AGuardianCharacter::GuardianStopRun(const FInputActionValue& Value)
{
	GuardianCurrentMoveSpeed = GuardianWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = GuardianCurrentMoveSpeed;
}

void AGuardianCharacter::GuardianJump(const FInputActionValue& Value)
{
	// Internal CharMoveComp-logic for checking if we can jump.
	if (CanJump())
	{
		// Jump if on the ground or allowed to double jump
		Super::Jump();
		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Warning, TEXT("Jumping! Current jump count: %d"), JumpCurrentCount);
		}
	}
}

void AGuardianCharacter::GuardianStopJump(const FInputActionValue& Value)
{
	Super::StopJumping();
	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stopped Jumping"));
	}
}

void AGuardianCharacter::GuardianLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (GuardianController != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * LookSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * LookSensitivity);
	}
}

void AGuardianCharacter::GuardianDeath(AActor* OtherActor)
{
	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("Guardian died in contact with %s!"), *OtherActor->GetName());
	}
	
	SetActorLocation(CurrentRespawnLocation);
}

void AGuardianCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (OtherActor && Cast<APlayerCheckpoint>(OtherActor))
	{
		CurrentRespawnLocation = OtherActor->GetActorLocation() + FVector(0, 0, 50);
		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Warning, TEXT("Guardian respawn location is: %s!"), *CurrentRespawnLocation.ToString());
		}
	}
	else if (OtherActor && Cast<ACharacter>(OtherActor)) // Assuming the Enemy AI are the only ACharacters.
	{
		GuardianDeath(OtherActor);
	}
}

void AGuardianCharacter::SetupStimulusSource()
{
	//Creates Stimulus Source for enemyAI
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (StimulusSource)
	{
		//Registers the Stimulus Source with the perception system
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterWithPerceptionSystem();
	}
}