// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/GuardianCharacter.h"
#include "Player/GuardianController.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Player/PlayerCheckpoint.h"

/**
	* Overview and Execution Order of the code:
	* 1. Constructor AGuardianCharacter() - When actor is instantiated (before Play mode).
	* 2. NotifyControllerChanged() - When the character gets a controller. (And when you exit Playmode)
	* 3. SetupPlayerInputComponent() - Runs when Unreal sets up input for the character.
	* 4. BeginPlay() - Character is now fully in the world, and Play Mode is started.
	* 
	* 5. not added, but if desired - Tick(DeltaTime) - Every frame and runs 60+ times per second.
	*
	* GuardianMove(), GuardianInteract(), etc. - Only runs when buttons are pressed.
 */

// Sets default values
AGuardianCharacter::AGuardianCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentRespawnLocation = GetActorLocation();
	
	// Setting up our collision capsule from CharacterMovementComponent, and making it the rootComponent
	GuardianCapsuleComponent = GetCapsuleComponent();
	GuardianCapsuleComponent->InitCapsuleSize(PlayerCapsuleColliderRadius, PlayerCapsuleColliderHalfHeight);

	// Have to set the root component!
	SetRootComponent(GuardianCapsuleComponent);
	
	// Now this below collision stuff is wack stuff, thank god we have ChatGPT:
		// PS. You can have custom collision channels too.
	
	// Set our collision type:
	// QueryOnly would do overlaps and traces, but block nothing; PhysicsOnly would be great for physics simulations,
	// NoCollision explains itself and QueryAndPhysics gets overlaps and blocking (best for characters).
	GuardianCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// What kind of object are we? ECC_Pawn works for Characters, ECC_WorldStatic are for Walls/Floor, ECC_WorldDynamic
	// are for things that move or interact. Use ECollisionChannel:: to find more options.
	GuardianCapsuleComponent->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);

	// What is our default response to collisions? Block. (ECR_Ignore, ECR_Block, ECR_MAX, and ECR_Overlap etc.)
	// Block makes sure we collide with most items (avoid falling through the ground).
	GuardianCapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Except... We don't want to block pickups and other dynamic objects! They're in the WorldDynamic channel,
	// and we set our response to that channel to be ECR_Overlap.
	GuardianCapsuleComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	
	// Setting up our Guardian mesh component (animated rig)
	GuardianMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GuardianMesh"));
	GuardianMeshComponent->SetupAttachment(GuardianCapsuleComponent);
	GuardianMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -PlayerCapsuleColliderHalfHeight));
	GuardianMeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// Finding, and setting the right skeletalMeshAsset.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GuardianMeshAsset(TEXT("SkeletalMesh'/Game/GuardianCharacter/augh.augh'"));
    if (GuardianMeshAsset.Succeeded())
    {
	    GuardianMeshComponent->SetSkeletalMesh(GuardianMeshAsset.Object);
    }

	// Apply the default ThirdPerson Animation Blueprint (if you want animations)
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(TEXT("AnimBlueprint'/Game/GuardianCharacter/Animations/ABP_Guardian.ABP_Guardian_C'"));
	if (AnimBPClass.Succeeded())
	{
		GuardianMeshComponent->SetAnimInstanceClass(AnimBPClass.Class); // Set the animation blueprint
	}

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Create the camera boom (spring arm) and attach it to the root
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GuardianCapsuleComponent);
	CameraBoom->TargetArmLength = CameraDistanceToPlayer; // Distance between camera and the player it follows
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create the follow camera and attach it to the boom's socket
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to the arm

	// Create the SphereCollision for pickups
	PickupRadiusSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupRadiusSphere->SetupAttachment(GuardianCapsuleComponent);
	PickupRadiusSphere->InitSphereRadius(PickupRadiusLength);

	// If you understand what we did above with the collision capsule, we're doing things a little bit different now
	// for the overlap sphere that helps run item stuff:
	PickupRadiusSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 
	PickupRadiusSphere->SetCollisionResponseToAllChannels(ECR_Ignore); // Default ignores all.
	PickupRadiusSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // Except dynamic objects.
	
	// Set up Character Movement Component
	if (GetCharacterMovement())
	{
		// Ensuring right movement mode and animation is played + gravity.
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->GravityScale = 1.0f; // Default gravity scale (adjust if needed)

		// Character orientation
		GetCharacterMovement()->bOrientRotationToMovement = true; // Turns character in movement direction
		GetCharacterMovement()->bUseControllerDesiredRotation = false;

		GuardianCurrentSpeed = GuardianMoveSpeed;
		
		// Walking
		GetCharacterMovement()->MaxWalkSpeed = GuardianCurrentSpeed; // Walking speed
		GetCharacterMovement()->MaxStepHeight = 80.f; // Max step height
		GetCharacterMovement()->SetWalkableFloorAngle(50.f);
		GetCharacterMovement()->bCanWalkOffLedges = true; // Allow ledge climbing or walking off edges

		// Jumping
		GetCharacterMovement()->JumpZVelocity = GuardianJumpStrength; // Jumping speed
		GetCharacterMovement()->AirControl = 0.2f; // Control mid-air
		JumpMaxCount = 3;
	}
    
	// Disable physics simulation on the capsule and mesh, but still use CharacterMovement
	GuardianCapsuleComponent->SetSimulatePhysics(false);
	GuardianMeshComponent->SetSimulatePhysics(false);

	// Just to test and practice logging:
	// Being mindful that floats have to be limited due too many decimal spaces: %.2f = 2 decimals, %.1f = 1 decimal.
	// And strings need a * in front of them, otherwise no print for you.
	// unsigned 32-bit ints would be %u.

	/*
	int32 Score = 125;
	float Health = 87.5f;
	FString PlayerName = "Guardian";

	UE_LOG(LogTemp, Warning, TEXT("Just doing some test logging, to try format specifiers!"));
	UE_LOG(LogTemp, Warning, TEXT("Guardian %s has %d points and %.1f health left!"), *PlayerName, Score, Health);
	
	UE_LOG(LogTemp, Warning, TEXT("💂 GuardianCharacter Constructed!"));
	*/
}

void AGuardianCharacter::DebugDraw() const
{
	// Debug forward movement direction
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

// Called when the game starts or when spawned
void AGuardianCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentRespawnLocation = GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("Guardian Mesh Collision: %s"), 
	*UEnum::GetValueAsString(GuardianCapsuleComponent->GetCollisionEnabled()));

	// Run timer for checking pickup items in given interval; and run the corresponding function CheckForNearbyPickups.
	GetWorld()->GetTimerManager().SetTimer(PickupTimerHandle, this, &AGuardianCharacter::CheckForNearbyPickups, PickupCheckTimeInterval, true);
}

/** This function runs very early when you start playMode, and whenever a controller is changed.
 * If we want to change controllers in-game and change the displayed HUD according to player input device, then
 * this needs to be expanded with functionality to reapply mapping contexts from GuardianController,
 * also the script which will ensure the right context is sent when using a controller VS when using a keyboard.
*/ 
void AGuardianCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	
	// This nasty piece of log (sneaky conditional statement) actually tries to find the controller that triggers
	// NotifyControllerChanged(). If GetController returns a name, we print that name, else we print nothing.
	UE_LOG(LogTemp, Warning, TEXT("💂 NotifyControllerChanged Called! Controller: %s"), 
		   (GetController() ? *GetController()->GetName() : TEXT("None")));
}

// Called to make sure the GuardianController is cast and found
void AGuardianCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	GuardianController = Cast<AGuardianController>(GetController());
	
	if (GuardianController)
	{
		UE_LOG(LogTemp, Warning, TEXT("💂 Found GuardianController from GuardianCharacter!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ GuardianController is NULL (from GuardianCharacter)!"));
	}

	/**
	 *	Just want to highlight that the method of loading assets dynamically in runtime via BeginPlay() and StaticLoadObject,
	 *	is different from the ConstructorHelper::FObjectFinder -> which can only be done inside constructors:
	 *
	 *	Just leaving the example here for how you would do it in the constructor with ConstructorHelper.
	 * 
	 *  static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveFinder(TEXT("InputAction'/Game/Input/IA_Move.IA_Move'"));
	 *  if (IA_MoveFinder.Succeeded()) { IA_Move = IA_MoveFinder.Object; }
	 *  else
	 *  {
	 * 	 UE_LOG(LogTemp, Error, TEXT("Failed to load IA_Move!"));
	 *  }
	 */
}

void AGuardianCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bDebugEnabled)
	{
		// DebugGroundCheck();
		DebugDraw();
	}
}

bool AGuardianCharacter::IsOnGround() const
{
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 150.f); // Check downwards by 100 units

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore self

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		FVector SurfaceNormal = HitResult.Normal;
		float SlopeAngle = FMath::Acos(FVector::DotProduct(SurfaceNormal, FVector(0,0,1))) * (180.0f / PI);

		if (SlopeAngle < 45.f)
		{
			return true;
		}
	}
	return false;
}

FVector AGuardianCharacter::GetFloorNormal() const
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 100); // Check ground

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
	{
		return HitResult.Normal; // Return surface normal
	}
	return FVector(0, 0, 1); // Default: Flat ground
}

void AGuardianCharacter::GuardianMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	// If movementInput is received, but player isn't moving, let's not run the code.
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

// We just switch out WalkSpeed in CharacterMovementComponent with our runSpeed. :)
void AGuardianCharacter::GuardianRun(const FInputActionValue& Value)
{
	GuardianCurrentSpeed = GuardianRunSpeed;
	GetCharacterMovement()->MaxWalkSpeed = GuardianCurrentSpeed;

	// TODO: Run functionality to be added. Just double the move speed or something -benjamin
}

// We just switch out WalkSpeed in CharacterMovementComponent with our walkSpeed. :)
void AGuardianCharacter::GuardianStopRun(const FInputActionValue& Value)
{
	GuardianCurrentSpeed = GuardianMoveSpeed;
	GetCharacterMovement()->MaxWalkSpeed = GuardianCurrentSpeed;
}

void AGuardianCharacter::GuardianJump(const FInputActionValue& Value)
{
	// Internal CMC-logic for checking if we can jump.
	if (CanJump())
	{
		// Jump if on the ground or allowed to double jump
		Super::Jump();
		UE_LOG(LogTemp, Warning, TEXT("Jumping! Current jump count: %d"), JumpCurrentCount);
	}
}

void AGuardianCharacter::GuardianStopJumping(const FInputActionValue& Value)
{
	Super::StopJumping();
	UE_LOG(LogTemp, Warning, TEXT("Stopped Jumping"));
}

void AGuardianCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	UE_LOG(LogTemp, Warning, TEXT("Landed! Resetting jump count."));
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

// Doesn't have a parameter, since other "StartInteract()" functions receive those parameters. WiP.
void AGuardianCharacter::GuardianInteract()
{
	
	/* TArray<AActor*> NearbyInteractables;
	// The below function just detects if the PlayerCollision overlaps with an AInteractableItem.
	PickupRadiusSphere->GetOverlappingActors(NearbyInteractables, APickup::StaticClass());

	for (AActor* Interactable : NearbyInteractables)
	{
		if (Interactable)
		{ 
			UE_LOG(LogTemp, Warning, TEXT("Guardian is interacting with %s!"), *Item->GetName());
		}
	} */
}

void AGuardianCharacter::GuardianDeath(AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Guardian died in contact with %s!"), *OtherActor->GetName());

	SetActorLocation(CurrentRespawnLocation);
}

void AGuardianCharacter::GuardianEscape(const FInputActionValue& InputActionValue)
{
	// TODO: Add pause screen functionality. Bound to "Esc" key on the keyboard and "Start" on controller.
	
}

void AGuardianCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor && Cast<APlayerCheckpoint>(OtherActor))
	{
		CurrentRespawnLocation = OtherActor->GetActorLocation() + FVector(0, 0, 20);
		UE_LOG(LogTemp, Warning, TEXT("Guardian respawn location is: %s!"), *CurrentRespawnLocation.ToString());
	}
	else if (OtherActor && Cast<ACharacter>(OtherActor))
	{
		GuardianDeath(OtherActor);
	}
}

void AGuardianCharacter::CheckForNearbyPickups()
{
	TArray<AActor*> NearbyWorldObjects;
	PickupRadiusSphere->GetOverlappingActors(NearbyWorldObjects, AWorldObject::StaticClass());

	for (AActor* WorldObject : NearbyWorldObjects)
	{
		if (WorldObject)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found WorldObject: %s"), *WorldObject->GetName());
		}
	}
}


// TODO: Overall remove the extra tap / hold functionality for Interact? ...but I do like the timer. -benjamin
// For hold interactions:
void AGuardianCharacter::StartInteract(const FInputActionValue& Value)
{
	StoredInteractHoldTime = 0.0f; // Reset timer
	GetWorld()->GetTimerManager().SetTimer(InteractHoldTimerHandle, this, &AGuardianCharacter::UpdateHoldTime, 0.05f, true);

	UE_LOG(LogTemp, Display, TEXT("Guardian started holding interact!"));
}

void AGuardianCharacter::StopInteract(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().ClearTimer(InteractHoldTimerHandle); // stop tracking timer

	if (StoredInteractHoldTime >= InteractHoldTimeThreshold) // For example, hold for 1.5 seconds = Long GuardianInteract, else Short GuardianInteract
	{
		PerformLongInteract();
	}
	else
	{
		PerformShortInteract();
	}

	UE_LOG(LogTemp, Display, TEXT("Guardian stopped interacting after %.2f seconds!"), StoredInteractHoldTime);
}

void AGuardianCharacter::UpdateHoldTime()
{
	StoredInteractHoldTime += 0.05f; // Increase hold time by timer interval
}

void AGuardianCharacter::PerformShortInteract()
{
	UE_LOG(LogTemp, Display, TEXT("Guardian does a quick interact!"));
	// Calling the function again to run the "tap" interact event again. Can also be swapped with OnInteract(false);
	GuardianInteract(); 
}

void AGuardianCharacter::PerformLongInteract()
{
	UE_LOG(LogTemp, Display, TEXT("Guardian does a long interact!"));
}

/** TODO-List Benjamin before 8th of April - for functionality and polish, and can be checked as done via the emote 👍 
 *	--- Core Functionality: 💡 ---
 *	1. Move anything with input from GuardianCharacter to the GuardianController 👍
 *	2. Make the playerMesh appear so we can see where we are. 👍
 *	3. Ensure the player can collide with BP_Interactable 👍
 *	Do a playtest and ensure this GuardianCharacter and GuardianController can be used by others without big issues. 👍
 *
 *	4. Modify CharacterMovementComponent, and do stuff = Jump + double Jump, pickups, interact, etc. 👍
 *	5. Moving Platforms functionality
 *	6. Checkpoints functionality - including full rework of Pickup, with derived InteractableItem & Checkpoints.
 *	7. Player Death functionality
 *	8. Add minimum functional LedgeClimb functionality.
 *
 *	--- For the polishing stage (after 8th of April): 🧼 ---
 *
 *	A. Add "PerchRadius" - the little extra bit the character can walk near a ledge to avoid falling off
 *	B. Make the player Animations run (assuming just adding a Static Mesh Component isn't enough). 👍
 *  C. Add Echolocation of items functionality - to help envision and see where key objects are in your vision.
 *  D. Clean up old unused code, or functionality that is not needed.
 */