// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCheckpoint.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

/** Worth knowing is that any class deriving off of AWorldObject runs its constructor to initialise components and apply
 * a default set of variables. The C++ implementation of doing things; the back-up plan, or Plan B.
 *
 * All components initialised here in the constructor will be inherited and already initialised for subclasses.
 * Inherited classes should only add new components or modify the existing components.
 */
APlayerCheckpoint::APlayerCheckpoint()
{
	// Set this actor to call Tick() every frame. 
	PrimaryActorTick.bCanEverTick = false;

	// Override parent class defaults as PlayerCheckpoint constructor defaults.
	bIsActivated = false;    // Checkpoints are not activated by default.
	bIsPickup = false;       // This isn't a regular pickup
	bIsInteractable = true;  // Player interacts with checkpoints.
	bEnableFloating = false; // Checkpoints should not float.
	bIsLightOn = false;      // Checkpoints should not have lights on until activated.

	ObjectMeshScale = FVector(1.75f, 1.75f, 1.75f); // Default Mesh Scale for Checkpoints.
	
	CapsuleHalfHeight = 300.f; // Default Capsule HalfHeight for Checkpoints.
	CapsuleRadius = 300.f;	// Default Capsule Radius for Checkpoints.

	ObjectLightLocation = FVector(0.f, 0.f, 0.f); // Default LightLocation for Checkpoints.
	LightIntensity = 3000.f; // Default LightIntensity for Checkpoints.
	LightAttenuationRadius = 1000.f; // Default LightAttenuationRadius for Checkpoints.
	LightSourceRadius = 500.f; // Default LightSourceRadius for Checkpoints.

	// Setting DebugColor and LightColour to Magenta / Purple, befitting the checkpoint.
	DebugColour = FColor::Magenta;
	LightColour = FColor::Purple;

// The below code is just loading the right ObjectMesh and ObjectMaterial "from scratch via C++", as a plan B.
// Plan A involves using DataAssets to load and run meshes, but since that is Unreal Engine stuff and not 100% C++, this
// is to prove it can be done fully in C++ too, while also serving as failsafe.
	
	// Using a local variable, to try loading the mesh and assign it to the instance variable ObjectMesh.
	TSoftObjectPtr<UStaticMesh> PlayerCheckpointMesh = TSoftObjectPtr<UStaticMesh>
	(FSoftObjectPath(TEXT("/Game/OurAssets/FaeCircle.FaeCircle")));

	// Have we found the mesh, and is it loaded in time?
	if (PlayerCheckpointMesh.IsValid())
	{
		ObjectMesh = PlayerCheckpointMesh.Get();
	}
	else
	{
		// Try loading the UStaticMesh, and this works, assign it. If not, run a log warning.
		if (UStaticMesh* LoadedCheckpointMesh = PlayerCheckpointMesh.LoadSynchronous())
		{
			ObjectMesh = LoadedCheckpointMesh;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Failed to lazily load PlayerCheckpointMesh in %s!"), *GetClass()->GetName());
		}
	}

	// Loading MeshMaterialInterface, as with the ObjectMesh and the SettingsAsset.
	TSoftObjectPtr<UMaterialInterface> PlayerCheckpointMaterial = TSoftObjectPtr<UMaterialInterface>
	(FSoftObjectPath(TEXT("/Game/Materials/M_Floating_Platform.M_Floating_Platform")));

	// Have we found the material, and is it loaded in time?
	if (PlayerCheckpointMaterial.IsValid())
	{
		ObjectMaterial = PlayerCheckpointMaterial.Get();
	}
	else
	{
		// Try loading the UMaterialInterface, and this works, assign it. If not, run a log warning.
		if (UMaterialInterface* LoadedCheckpointMaterial = PlayerCheckpointMaterial.LoadSynchronous())
		{
			ObjectMaterial = LoadedCheckpointMaterial;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Failed to lazily load PlayerCheckpointMaterial in %s!"), *GetClass()->GetName());
		}
	}

	// Assigning the mesh and the material to the StaticMeshComponent if the mesh is valid.
	if (ObjectMesh.IsValid() && ObjectMeshComp)
	{
		// If valid, set the mesh to the component
		ObjectMeshComp->SetStaticMesh(ObjectMesh.Get());

		// Set the new MeshScale if a mesh is found
		ObjectMeshComp->SetWorldScale3D(ObjectMeshScale);

		if (PlayerCheckpointMaterial.IsValid())
		{
			// If valid, set the material to the component
			ObjectMeshComp->SetMaterial(0, ObjectMaterial.Get());
		}
	}
	if (ObjectCapsuleComp)
	{
		// ->InitCapsuleSize here since we're initialising the component and all for the first time. Performant over SetCapsuleSize.
		ObjectCapsuleComp->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);

		// Setting constructor defaults for the overlap capsule, to adjust it according to the object
		ObjectCapsuleComp->SetHiddenInGame(bDebugCapsuleVisibility);
	}
}

void APlayerCheckpoint::PlayerEntersInteractable()
{
	if (bDebugEnabled)
	{
		// Any light toggling and debug printing happen in the superclass function.
		Super::PlayerEntersInteractable(); 
	}

	// Usually for checkpoints - if activated and toggled to be one-time use, don't do anything during player overlaps.
	if (bIsActivated && bOneTimeUseOnly) return;

	// We don't want to run the superclass function twice if debug is enabled when we have already run it once above.
	if (!bDebugEnabled) 
	{
		Super::PlayerEntersInteractable();
	}

	if (!bIsActivated)
	{
		// The object is now activated. If oneTimeUse is enabled, this stays true.
		bIsActivated = true;
	}
	else
	{
		// Flagging the object as inactive / deactivated.
		bIsActivated = false;
	}
}

void APlayerCheckpoint::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if it's ACharacter
	if (const ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		// Check if this character is controlled by a player (eliminate enemy AI from the logic)
		if (Cast<APlayerController>(Character->GetController()))
		{
			if (bDebugEnabled)
			{
				FString objectName = this->GetName();
	
				check(GEngine != nullptr);
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, DebugColour, TEXT("Player leaves checkpoint! ... remember me as " + objectName));
			}
		}
	}
}