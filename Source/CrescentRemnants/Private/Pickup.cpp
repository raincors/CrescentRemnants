// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "WorldObjectSettings.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/CoreSettings.h"
#include "GameFramework/Character.h"

/** Worth knowing is that any class deriving off of AWorldObject, runs its constructor to initialize components and apply
 * a default set of variables. The C++ implementation of doing things; the back-up plan, or Plan B.
 *
 * All components initialized here in the constructor, will be inherited and already initialized for subclasses.
 * Inherited classes should only add new components or modify the existing components.
 */
APickup::APickup()
{
	// Set this actor to call Tick() every frame. Set to false as default.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create StaticMeshComponent, and attaching it to the root
	ObjectMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupObjectMesh"));
	ObjectMeshComp->SetupAttachment(ObjectRoot);
	
	// Loading ObjectMesh, same as with loading Settings.
	TSoftObjectPtr<UStaticMesh> PickupMesh = TSoftObjectPtr<UStaticMesh>
	(FSoftObjectPath(TEXT("/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid")));

	// Have we found the mesh, and is it loaded in time?
	if (PickupMesh.IsValid())
	{
		ObjectMesh = PickupMesh.Get();
	}
	else
	{
		// Try load the UStaticMesh, and this works, assign it. If not, run a log warning.
		if (UStaticMesh* LoadedPickupMesh = PickupMesh.LoadSynchronous())
		{
			ObjectMesh = LoadedPickupMesh;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Failed to lazily load PickupMesh in %s!"), *GetClass()->GetName());
		}
	}

	// Loading MeshMaterialInterface, as with the ObjectMesh and the SettingsAsset.
	TSoftObjectPtr<UMaterialInterface> PickupMaterial = TSoftObjectPtr<UMaterialInterface>
	(FSoftObjectPath(TEXT("/Game/StarterContent/Materials/M_Brick_Clay_New.M_Brick_Clay_New")));

	// Have we found the material, and is it loaded in time?
	if (PickupMaterial.IsValid())
	{
		ObjectMaterial = PickupMaterial.Get();
	}
	else
	{
		// Try load the UMaterialInterface, and this works, assign it. If not, run a log warning.
		if (UMaterialInterface* LoadedPickupMaterial = PickupMaterial.LoadSynchronous())
		{
			ObjectMaterial = LoadedPickupMaterial;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Failed to lazily load PickupMaterial in %s!"), *GetClass()->GetName());
		}
	}

	// Assigning the mesh and the material to the StaticMeshComponent, if the mesh is valid.
	if (PickupMesh.IsValid())
	{
		// If valid, set the mesh to the component
		ObjectMeshComp->SetStaticMesh(ObjectMesh.Get());
		// Pickups shouldn't have MeshCollision enabled (derived from AWorldObject). This saves on performance.
		ObjectMeshComp->SetCollisionProfileName(AWorldObject::GetMeshCollisionTag());

		// Set Mesh Rotation and Scale if a mesh is found
		ObjectMeshComp->SetWorldRotation(ObjectMeshRotation);
		ObjectMeshComp->SetWorldScale3D(ObjectMeshScale);

		if (PickupMaterial.IsValid())
		{
			// If valid, set the material to the component
			ObjectMeshComp->SetMaterial(0, ObjectMaterial.Get());
		}
	}

	// Create CollisionSphere Component (to enable overlaps, and interactions)
	ObjectCapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("OverlapCapsule"));
	ObjectCapsuleComp->SetupAttachment(ObjectRoot);
	ObjectCapsuleComp->InitCapsuleSize(CapsuleRadius, CapsuleHalfHeight);

	// Setting constructor defaults for the overlap capsule, to adjust it according to the object
	ObjectCapsuleComp->SetRelativeLocation(CapsuleLocation);
	ObjectCapsuleComp->SetRelativeRotation(CapsuleRotation);
	ObjectCapsuleComp->SetRelativeScale3D(CapsuleScale);
	ObjectCapsuleComp->SetHiddenInGame(bDebugCapsuleVisibility);

	// Enabling overlap events, and making sure they only overlap with dynamic objects (ideally just the player)
	ObjectCapsuleComp->SetGenerateOverlapEvents(true);

	// Nasty stuff, but the below code is about how we do collision and what our component should collide with.
	// You can read more inside the constructor of GuardianCharacter, how we do it with the player.

	// QueryOnly means we detect traces and overlap, but don't block anything.
	ObjectCapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// We set our capsuleComponent to be ECC_WorldDynamic - a WorldObject in motion pretty much.
	ObjectCapsuleComp->SetCollisionObjectType(ECC_WorldDynamic);
	// Default response to other collision channels is to ignore them. Pickups don't care for other collisions.
	ObjectCapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	// ...Except ECC_Pawn, the closest we have to ACharacter, where we do want to overlap.
	ObjectCapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Create PointLight Component
	ObjectPointLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	ObjectPointLightComp->SetupAttachment(ObjectRoot);
	ObjectPointLightComp->SetRelativeLocation(ObjectLightLocation);
	
	// Is light on or off? According to default settings or instance override.
	ObjectPointLightComp->SetVisibility(bIsLightOn);

	// Setting default light outreach, intensity and colour.
	ObjectPointLightComp->SetAttenuationRadius(LightAttenuationRadius);
	ObjectPointLightComp->SetIntensity(LightIntensity);
	ObjectPointLightComp->SetLightColor(LightColour);
	ObjectPointLightComp->SetSourceRadius(LightSourceRadius);

	// Create Audio Component
	ObjectAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	ObjectAudioComp->SetupAttachment(ObjectRoot);
	// Ensuring audio doesn't play when the object is constructed or spawned.
	ObjectAudioComp->bAutoActivate = false;
	
	// --- TODO: Fix asset path here and uncomment when we're ready to add a USoundBase :) ---
	/* TSoftObjectPtr<USoundBase> PickupSound = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/PathToUSoundBaseHere")));
	if (PickupSound.IsValid())
	{
		// Assign the USoundBase to the AudioComponent
		ObjectSound = PickupSound.Get();
		ObjectAudioComp->Sound = ObjectSound.Get();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ Can't find USoundBase asset in %s!"), *GetClass()->GetName());
	} */
}

/** Settings applied here from the SettingsAsset (UDataAsset):
 * 
 * Bool: bIsInteractable
 * 
 * Debug: DebugCapsuleVisibility DebugTextColour
 * 
 * Floating: bFloatingEnabled, DefaultFloatingHeight, DefaultFloatingSpeed
 * 
 * Mesh: DefaultMesh(), DefaultObjectMeshRotation, DefaultObjectMeshScale
 * 
 * Material: DefaultMaterial().
 * 
 */
bool APickup::UseWorldObjectAssetSettings()
{
	if (!Super::UseWorldObjectAssetSettings())
	{
		return false;
	}
	
	// Setting bools.
	bIsInteractable = SettingsAsset->bIsInteractable;
	
	// Setting debug settings:
	bDebugCapsuleVisibility = SettingsAsset->bDebugIsCapsuleCollisionVisible;
	DebugColour = SettingsAsset->DefaultDebugTextColour;
	LightColour	= SettingsAsset->DefaultLightColour;

	// Float Settings:
	bEnableFloating = SettingsAsset->bEnableFloating;
	if (bEnableFloating)
	{
		// Needs to activated as well. Use this to e.g. disable pickups far away or similarly.
		bIsActivated = SettingsAsset->bIsActivated;
		
		FloatingDistance = SettingsAsset->DefaultFloatingDistance;
		FloatingSpeed = SettingsAsset->DefaultFloatingSpeed;
	}
	
	// Apply the default mesh
	if (SettingsAsset->DefaultMesh.IsValid())
	{
		ObjectMesh = SettingsAsset->DefaultMesh.LoadSynchronous();
		ObjectMeshComp->SetStaticMesh(ObjectMesh.Get());

		// Rotating or adjusting the mesh according to settingAsset.
		ObjectMeshRotation = SettingsAsset->DefaultObjectMeshRotation;
		ObjectMeshScale = SettingsAsset->DefaultObjectMeshScale;

		// Set Mesh Rotation and Scale if a mesh is found
		ObjectMeshComp->SetWorldRotation(ObjectMeshRotation);
		ObjectMeshComp->SetWorldScale3D(ObjectMeshScale);
		
		// Apply the default material
		if (SettingsAsset->DefaultMaterial.IsValid())
		{
			ObjectMaterial = SettingsAsset->DefaultMaterial.LoadSynchronous();
			ObjectMeshComp->SetMaterial(0, ObjectMaterial.Get());
		}
	}

	// Capsule Comp Variables
	CapsuleRadius = SettingsAsset->DefaultCapsuleRadius;
	CapsuleHalfHeight = SettingsAsset->DefaultCapsuleHalfHeight;
	bDebugCapsuleVisibility = SettingsAsset->bDebugIsCapsuleCollisionVisible;
	
	// Apply the default collision capsule
	if (ObjectCapsuleComp)
	{
		ObjectCapsuleComp->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
		ObjectCapsuleComp->SetVisibility(true, true);
		ObjectCapsuleComp->SetHiddenInGame(bDebugCapsuleVisibility);
	}

	// Point Light Variables
	bIsLightOn = SettingsAsset->bIsLightOn;
	ObjectLightLocation = SettingsAsset->DefaultLightLocation;
	LightIntensity = SettingsAsset->DefaultLightIntensity;
	LightAttenuationRadius = SettingsAsset->DefaultAttenuationRadius;
	LightSourceRadius = SettingsAsset->DefaultLightSourceRadius;
	LightColour = SettingsAsset->DefaultLightColour;

	// Apply default light Settings (if enabled)
	if (ObjectPointLightComp)
	{
		ObjectPointLightComp->SetVisibility(bIsLightOn);
		ObjectPointLightComp->SetRelativeLocation(ObjectLightLocation);
		ObjectPointLightComp->SetIntensity(LightIntensity);
		ObjectPointLightComp->SetAttenuationRadius(LightAttenuationRadius);
		ObjectPointLightComp->SetSourceRadius(LightSourceRadius);
		ObjectPointLightComp->SetLightColor(LightColour);
	}

	// Apply optional sound
	if (ObjectAudioComp && SettingsAsset->DefaultSound.IsValid())
	{
		ObjectAudioComp->SetSound(SettingsAsset->DefaultSound.LoadSynchronous());
		ObjectAudioComp->bAutoActivate = SettingsAsset->bActivateOnSpawn;
	}

	return true;
}

// Setting all our variables in BeginPlay - ensuring any updates in the Editor are updated in PlayMode.
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// The function for floating. Used by Pickup, Checkpoint(?) and Platforms + possibly Hazards. To be overridden.
void APickup::Float()
{
	float Time = GetWorld()->TimeSeconds;
	float Offset = FMath::Sin(Time * FloatingSpeed) * FloatingDistance;
	
	FVector NewLocation = InitialLocation + FVector(0.f, 0.f, Offset);
	SetActorLocation(NewLocation);
}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Floating needs to be enabled
	if (bEnableFloating)
	{
		// And the object needs to be activated, or not an interactable
		if (bIsActivated || !bIsInteractable)
		{
			Float();
		}
	}
}

#if WITH_EDITOR

/** Since all our variables are set with UPROPERTY and can be edited in the Editor, this function helps make sure
 * that when we change variables in the Editor, the changes will show up instantly. 
 *
 * @param PropertyChangedEvent = Unreal Engine's own built-in event system to check the top-level UPROPERTY we've changed.
 * It doesn't work as well for fields inside e.g. PointLight or ObjectCapsuleComp, but do use if you switch out any components,
 * like mesh or material, or different light component.
 */
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// Debugging if Debugging is enabled.
	if (bDebugEnabled)
	{
		if (ObjectCapsuleComp && bDebugCapsuleVisibility)
		{
			DrawDebugCapsule(GetWorld(), GetActorLocation(), CapsuleHalfHeight, CapsuleRadius,
		CapsuleRotation.Quaternion(), FColor::Red, false, -1, 0, 1.f);
		}
	}

	if (SettingsAsset.IsValid())
	{
		if (SettingsAsset->bAllowInstancesToOverride)
		{
			bAllowSettingsOverride = true;
		}
		else
		{
			bAllowSettingsOverride = false;
		}
	}
	
	// Don't continue with any code if manual overrides aren't enabled on this instance.
	if (!bAllowSettingsOverride) return;
	
	FName ChangedProperty = PropertyChangedEvent.Property->GetFName();

	// Changing the ObjectMesh 
	if (PropertyChangedEvent.Property && ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, ObjectMesh))
	{
		if (ObjectMesh.IsValid() || ObjectMesh.ToSoftObjectPath().IsValid())
		{
			UStaticMesh* LoadedMesh = ObjectMesh.LoadSynchronous();
			ObjectMeshComp->SetStaticMesh(LoadedMesh);
			UE_LOG(LogTemp, Warning, TEXT("👍 ObjectMesh loaded in PostEditChangeProperty in %s!"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Can't update the static mesh in PostEditChangeProperty! ObjectMesh is null! %s"), *GetName())
		}
		// Optionally, mark the render state dirty to force a refresh:
		ObjectMeshComp->MarkRenderStateDirty();
	}

	// Changing the ObjectMaterial - Keep in mind this only works with materials inside the Content or Game folder.
	if (PropertyChangedEvent.Property && ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, ObjectMaterial))
	{
		// Checking that the material is valid, and that the path to the material is valid too.
		if (ObjectMaterial.IsValid() && ObjectMaterial.ToSoftObjectPath().IsValid())
		{
			if (UMaterialInterface* LoadedMaterial = ObjectMaterial.LoadSynchronous())
			{
				ObjectMeshComp->SetMaterial(0, LoadedMaterial);
				UE_LOG(LogTemp, Warning, TEXT("👍 Material updated in PostEditChangeProperty! %s"), *GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to synchronously load ObjectMaterial in PostEditChangeProperty! %s"), *GetName());
		}
		// Optionally, mark the render state dirty to force a refresh:
		ObjectMeshComp->MarkRenderStateDirty();
	}
	
	
	// This isn't pretty, but Unreal has no native in-built way to check fields inside components.
	// Check if any field in ObjectCapsuleComp has changed.
	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, CapsuleRadius) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, CapsuleHalfHeight) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, CapsuleLocation) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, CapsuleRotation) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, CapsuleScale) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, bDebugCapsuleVisibility))
	{
		// Check if Capsule Comp not nullptr
		if (ObjectCapsuleComp)
		{
			// TODO: Fix weird issues - the editor does not like it when we run the radius and half-height code like this.
			
			// Enforce the capsule size constraints - to avoid issues with possible bugs / crashes regarding capsules.
			if (CapsuleHalfHeight <= CapsuleRadius)
			{
				CapsuleHalfHeight = CapsuleRadius;
			}
			else if (CapsuleRadius >= CapsuleHalfHeight)
			{
				CapsuleRadius = CapsuleHalfHeight;
			}
			
			ObjectCapsuleComp->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
		
			ObjectCapsuleComp->SetRelativeLocation(FVector(CapsuleLocation));
			ObjectCapsuleComp->SetRelativeRotation(FRotator(CapsuleRotation));
			ObjectCapsuleComp->SetRelativeScale3D(FVector(CapsuleScale));

			// If you don't want to see the capsule in Playmode.
			ObjectCapsuleComp->SetHiddenInGame(bDebugCapsuleVisibility);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ ObjectCapsuleComp is null in PostEditChangeProperty! %s"), *GetName())
		}
	}

	// Check if any field in ObjectPointLightComp has changed.
	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, bIsLightOn) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, LightIntensity) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, LightColour) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, LightSourceRadius) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, LightAttenuationRadius))
	{
		// Check if Point Light Comp not nullptr
		if (ObjectPointLightComp)
		{
			ObjectPointLightComp->SetRelativeLocation(ObjectLightLocation);
			ObjectPointLightComp->SetVisibility(bIsLightOn);
			ObjectPointLightComp->SetAttenuationRadius(LightAttenuationRadius);
			ObjectPointLightComp->SetIntensity(LightIntensity);
			ObjectPointLightComp->SetLightColor(LightColour);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ ObjectPointLightComp is null in PostEditChangeProperty! %s"), *GetName())
		}
	}
}

#endif

void APickup::PickupItem()
{
	if (bDebugEnabled)
	{
		FString objectName = this->GetName();
		check(GEngine != nullptr);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, DebugColour,TEXT("I'm being picked up! ... remember me as " + objectName));
	}
	if (bDestroyOnPickup || SettingsAsset->bOneTimeUse)
	{
		Destroy();
	}
}

// A default function for interactableItem and Checkpoint to use, APickup should run PickupItem() instead.
// This function can be run 
void APickup::PlayerEntersInteractable()
{
	if (bDebugEnabled)
	{
		FString objectName = this->GetName();
		check(GEngine != nullptr);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, DebugColour,TEXT("...player enters my InteractableZone. " + objectName));
	}
	
	// Toggle the light, if enabled on the object
	if (SettingsAsset && SettingsAsset->bInteractionTogglesLight)
	{
		bIsLightOn = !bIsLightOn;
	}
}

void APickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							bool bFromSweep, const FHitResult& SweepResult)
{
	// Looking for ACharacter, since we have no other ACharacters in the level, and it's less expensive
	if (OtherActor && Cast<ACharacter>(OtherActor))
	{
		// InteractableItems and PlayerCheckpoint should have this bool enabled, avoiding 
		if (bIsInteractable)
		{
			PlayerEntersInteractable();
		}
		else
		{
			PickupItem();
		}
	}
}
