// Fill out your copyright notice in the Description page of Project Settings.

#include "MovingPlatform.h"
#include "Components/CapsuleComponent.h"

/** Worth knowing is that any class deriving off of AWorldObject runs its constructor to initialise components and apply
 * a default set of variables. The C++ implementation of doing things; the back-up plan, or Plan B.
 *
 * All components initialised here in the constructor will be inherited and already initialised for subclasses.
 * Inherited classes should only add new components or modify the existing components.
 */
AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Override parent class defaults as PlayerCheckpoint constructor defaults.
	bDebugEnabled = false;			// Moving Platforms should not run debug by default to avoid log spam.
	bIsPickup = false;				// This isn't a pickup
	bIsInteractable = true;			// Player interacts with Platforms.
	bUseSineWave = false;			// Moving Platforms do generally not use sine wave movement.

	ObjectMeshOffset = FVector(0, 0, -640.f); // Default Mesh Location Offset for Platforms.
	ObjectMeshScale = FVector(1.f, 1.f, 1.f); // Default Mesh Scale for Platforms.

	CapsuleLocationOffset = FVector(0.f, 0.f, 0.f); // Default Capsule Location for Platforms.
	CapsuleRotationOffset = FRotator(0.f, 0.f, 90.f); // Default Capsule Rotation for Platforms.
	CapsuleHalfHeight = 300.f; // Default Capsule HalfHeight for Platforms.
	CapsuleRadius = 300.f;	// Default Capsule Radius for Platforms.

	ObjectLightLocation = FVector(0.f, 0.f, 0.f); // Default LightLocation for Platforms.
	LightIntensity = 1000.f; // Default LightIntensity for Platforms.
	LightAttenuationRadius = 600.f; // Default LightAttenuationRadius for Platforms.
	LightSourceRadius = 200.f; // Default LightSourceRadius for Platforms.

	// Setting DebugColor and LightColour.
	DebugColour = FColor::Orange;
	LightColour = FColor::Orange;
	
// The below code is just loading the right ObjectMesh and ObjectMaterial "from scratch via C++", as a plan B.
// Plan A involves using DataAssets to load and run meshes, but since that is Unreal Engine stuff and not 100% C++, this
// is to prove it can be done fully in C++ too, while serving as failsafe.

	// Using a local variable to try loading the mesh and assign it to the instance variable ObjectMesh.
	TSoftObjectPtr<UStaticMesh> PlatformMesh = TSoftObjectPtr<UStaticMesh>
	(FSoftObjectPath(TEXT("/Game/OurAssets/Floating_Platform.Floating_Platform")));

	// Have we found the mesh, and is it loaded in time?
	if (PlatformMesh.IsValid())
	{
		ObjectMesh = PlatformMesh.Get();
	}
	else
	{
		// Try loading the UStaticMesh, and this works, assign it. If not, run a log warning.
		if (UStaticMesh* LoadedPlatformMesh = PlatformMesh.LoadSynchronous())
		{
			ObjectMesh = LoadedPlatformMesh;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Failed to lazily load PlatformMesh in %s!"), *GetClass()->GetName());
		}
	}

	// Loading MeshMaterialInterface, as with the ObjectMesh and the SettingsAsset.
	TSoftObjectPtr<UMaterialInterface> PlatformMaterial = TSoftObjectPtr<UMaterialInterface>
	(FSoftObjectPath(TEXT("/Game/Materials/M_Floating_Platform.M_Floating_Platform")));

	// Have we found the material, and is it loaded in time?
	if (PlatformMaterial.IsValid())
	{
		ObjectMaterial = PlatformMaterial.Get();
	}
	else
	{
		// Try loading the UMaterialInterface, and this works, assign it. If not, run a log warning.
		if (UMaterialInterface* LoadedPlatformMaterial = PlatformMaterial.LoadSynchronous())
		{
			ObjectMaterial = LoadedPlatformMaterial;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Failed to lazily load PlatformMaterial in %s!"), *GetClass()->GetName());
		}
	}

	// Assigning the mesh and the material if the mesh and the ObjectMeshComponent are valid.
	if (ObjectMesh.IsValid() && ObjectMeshComp)
	{
		// If valid, set the mesh to the component
		ObjectMeshComp->SetStaticMesh(ObjectMesh.Get());

		// Enable complex collision on the mesh
		ObjectMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		// Platforms should have MeshCollision enabled.
		ObjectMeshComp->SetCollisionProfileName(GetMeshCollisionTag());

		// Block all channels by default
		ObjectMeshComp->SetCollisionResponseToAllChannels(ECR_Block);

		if (PlatformMaterial.IsValid())
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
		ObjectCapsuleComp->SetRelativeLocation(CapsuleLocationOffset);
		ObjectCapsuleComp->SetRelativeRotation(CapsuleRotationOffset);
		ObjectCapsuleComp->SetHiddenInGame(bDebugCapsuleVisibility);

		// QueryOnly means we detect traces and overlap but don't block anything.
		ObjectCapsuleComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		// The default response to other collision channels is to block them. Platforms care for other collisions.
		ObjectCapsuleComp->SetCollisionResponseToAllChannels(ECR_Block);
	}
}

bool AMovingPlatform::UseWorldObjectAssetSettings()
{
	if (!Super::UseWorldObjectAssetSettings())
	{
		return false;
	}
	return true;
}

void AMovingPlatform::UnpauseMovement()
{
	Super::UnpauseMovement();
}

void AMovingPlatform::Float()
{
	Super::Float();
}
