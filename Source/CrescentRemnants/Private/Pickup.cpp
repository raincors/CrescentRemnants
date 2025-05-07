// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable All - it's to disable warnings with comments where I've used doxygen formatting outside the intended purpose.
#include "Pickup.h"
#include "WorldObjectSettings.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Character.h"

/** Worth knowing is that any class deriving off of AWorldObject runs its constructor to initialise components and apply
 * a default set of variables. The C++ implementation of doing things; the back-up plan, or Plan B.
 *
 * All components initialised here in the constructor will be inherited and already initialised for subclasses.
 * Inherited classes should only add new components or modify the existing components.
 */
APickup::APickup()
{
	// Set this actor to call Tick() every frame. Set to false as default.
	PrimaryActorTick.bCanEverTick = true;

	// Setting a APickup constructor default DebugColour and LightColour
	DebugColour = FColor::Yellow;
	LightColour = FColor::Yellow;

	// Setting current float position
	CurrentFloatPosition = FloatStartPosition;
	// Setting current float direction
	if (bStartInPositiveDirection)
	{
		bIsMovingInPositiveDirection = true;
	}
	
// The below code is just loading the right component and all that "from scratch via C++", as a plan B.
// Plan A involves using DataAssets to load and run stuff, but since that is Unreal Engine stuff and not 100% C++, this
// is to prove it can be done fully in C++ too, while serving as failsafe.

	// Create a StaticMeshComponent and attaching it to the root
	ObjectMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupObjectMesh"));
	ObjectMeshComp->SetupAttachment(ObjectRoot);
	
	// StaticMesh
	TSoftObjectPtr<UStaticMesh> PickupMesh = TSoftObjectPtr<UStaticMesh>
	(FSoftObjectPath(TEXT("/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid")));

	// Have we found the mesh, and is it loaded in time?
	if (PickupMesh.IsValid())
	{
		ObjectMesh = PickupMesh.Get();
	}
	else
	{
		// Try loading the UStaticMesh, and this works, assign it. If not, run a log warning.
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
		// Try loading the UMaterialInterface, and this works, assign it. If not, run a log warning.
		if (UMaterialInterface* LoadedPickupMaterial = PickupMaterial.LoadSynchronous())
		{
			ObjectMaterial = LoadedPickupMaterial;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ Failed to lazily load PickupMaterial in %s!"), *GetClass()->GetName());
		}
	}

	// Assigning the mesh and the material to the StaticMeshComponent if the mesh is valid.
	if (ObjectMesh.IsValid() && ObjectMeshComp->IsValidLowLevel())
	{
		// If valid, set the mesh to the component
		ObjectMeshComp->SetStaticMesh(ObjectMesh.Get());
		// Pickups shouldn't have MeshCollision enabled (derived from AWorldObject). This saves on performance.
		ObjectMeshComp->SetCollisionProfileName(GetMeshCollisionTag());

		// Set Mesh Location, Rotation and Scale if a mesh is found
		ObjectMeshComp->SetRelativeLocation(ObjectMeshOffset);
		ObjectMeshComp->SetRelativeRotation(ObjectMeshRotation);
		ObjectMeshComp->SetRelativeScale3D(ObjectMeshScale);

		if (PickupMaterial.IsValid())
		{
			// If valid, set the material to the component
			ObjectMeshComp->SetMaterial(0, ObjectMaterial.Get());
		}
	}

	// CapsuleComponent
	ObjectCapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("OverlapCapsule"));
	ObjectCapsuleComp->SetupAttachment(ObjectRoot);
	// ->InitCapsuleSize here since we're initialising the component and all for the first time. Performant over SetCapsuleSize.
	ObjectCapsuleComp->InitCapsuleSize(CapsuleRadius, CapsuleHalfHeight);

	// Setting constructor defaults for the overlap capsule, to adjust it according to the object
	ObjectCapsuleComp->SetRelativeLocation(CapsuleLocationOffset);
	ObjectCapsuleComp->SetRelativeRotation(CapsuleRotationOffset);
	ObjectCapsuleComp->SetHiddenInGame(bDebugCapsuleVisibility);

	// Enabling overlap events and making sure they only overlap with dynamic objects (ideally just the player)
	ObjectCapsuleComp->SetGenerateOverlapEvents(true);

	// Nasty stuff, but the below code is about how we do collision and what our component should collide with.
	// You can read more inside the constructor of GuardianCharacter, how we do it with the player.

	// QueryOnly means we detect traces and overlap but don't block anything.
	ObjectCapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// We set our capsuleComponent to be ECC_WorldDynamic - a WorldObject in motion pretty much.
	ObjectCapsuleComp->SetCollisionObjectType(ECC_WorldDynamic);
	// The default response to other collision channels is to ignore them. Pickups don't care for other collisions.
	ObjectCapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	// ...Except ECC_Pawn, the closest we have to ACharacter, where we do want to overlap.
	ObjectCapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	

	// PointLightComponent
	ObjectPointLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	ObjectPointLightComp->SetupAttachment(ObjectRoot);
	ObjectPointLightComp->SetRelativeLocation(ObjectLightLocation);
	
	// Is the light on or off? According to default settings or instance override.
	ObjectPointLightComp->SetVisibility(bIsLightOn);

	// Setting default light outreach, intensity and colour.
	ObjectPointLightComp->SetAttenuationRadius(LightAttenuationRadius);
	ObjectPointLightComp->SetIntensity(LightIntensity);
	ObjectPointLightComp->SetLightColor(LightColour);
	ObjectPointLightComp->SetSourceRadius(LightSourceRadius);
}

/** Additional flags and variables modified from the SettingsAsset (UDataAsset):
 *
 * @param bIsPickup					Is this a pickup the player collects? (Destroys the object)
 * @param bIsInteractable			Is this an object the player interacts with?
 * @param bEnableFloating			Is this object floating?
 * @param -
 * @param ObjectMesh				The UStaticMesh the object has.
 * @param ObjectMeshRotation		Rotation to apply to the ObjectMesh.
 * @param ObjectMeshScale			Scale to apply to the ObjectMesh.
 * @param ObjectMaterial			The UMaterialInterface the object has (Material or MaterialInstance).
 * @param -
 * @param CapsuleHalfHeight			Rotation to apply to the Root component.
 * @param CapsuleRadius				Scale to apply to the Root component.
 * @param bDebugCapsuleVisibility	Is the capsuleComponent visible in-game?
 * @param -
 * @param FloatAxis					Which axis does the object float in? Default is Z.
 * @param FloatStartPosition		Where in the float range does the object start?
 * @param bStartInPositiveDirection Does this object start floating in a positive direction?
 * @param bUseSineWave				Float in a sine wave (smooth), or ping-pong (consistent)? 
 * @param FloatingDistance			What is the default float distance?
 * @param FloatingSpeed				How quickly does this object float?
 * @param -
 * @param bIsLightOn				Is the PointLightComponent turned on?
 * @param ObjectLightLocation		Where is the PointLight according to the objectTransform?
 * @param LightIntensity			How strong is the light intensity?
 * @param LightAttenuationRadius	How far away does the light reach? NB: Can get expensive!
 * @param LightSourceRadius			How far does the innermost light source (100% brightness) reach out?
 * @param LightColour				What colour is the light?
 */
bool APickup::UseWorldObjectAssetSettings()
{
	// If WorldObject can't find dataAssetSettings, don't use them.
	if (!Super::UseWorldObjectAssetSettings())
	{
		return false;
	}
	
	// Object Bool Settings (new to Pickup, not inherited from AWorldObject)
	bIsPickup = SettingsAsset->bIsPickup;
	bIsInteractable = SettingsAsset->bIsInteractable;
	
	bEnableFloating = SettingsAsset->bEnableFloating;

	bIsLightOn = SettingsAsset->bIsLightOn;
	bOverlapTogglesLight = SettingsAsset->bOverlapTogglesLight;

	if (ObjectMeshComp)
	{
		// ...and not using custom float settings on the instance.
		if (!bUseCustomMeshSettings)
		{
			// Leaving this property assignment outside the function, since it is only through this codeflow we want to use it.
			bUseCustomMeshSettings = SettingsAsset->bUseCustomMeshSettings;

#if WITH_EDITOR
			// Calling our new retrieval of float settings function 
			RetrieveMeshAssetSettings();
#endif
			
			// Set Mesh Location, Rotation and Scale if a mesh is found
			ObjectMeshComp->SetRelativeLocation(ObjectMeshOffset);
			ObjectMeshComp->SetRelativeRotation(ObjectMeshRotation);
			ObjectMeshComp->SetRelativeScale3D(ObjectMeshScale);

			// StaticMeshComponent Settings
			if (ObjectMesh.IsValid())
			{
				ObjectMeshComp->SetStaticMesh(ObjectMesh.Get());
		
				// Apply the default material
				if (ObjectMaterial.IsValid())
				{
					ObjectMeshComp->SetMaterial(0, ObjectMaterial.Get());
				}
			}
		}
	}
		
	// CapsuleComponent Settings
	CapsuleLocationOffset = SettingsAsset->DefaultCapsuleLocation;
	CapsuleRotationOffset =	SettingsAsset->DefaultCapsuleRotation;
	
	CapsuleRadius = SettingsAsset->DefaultCapsuleRadius;
	CapsuleHalfHeight = SettingsAsset->DefaultCapsuleHalfHeight;
	bDebugCapsuleVisibility = SettingsAsset->bDebugIsCapsuleCollisionVisible;
	
	// Apply the default collision capsule
	if (ObjectCapsuleComp)
	{
		ObjectCapsuleComp->SetRelativeLocation(CapsuleLocationOffset);
		ObjectCapsuleComp->SetRelativeRotation(CapsuleRotationOffset);
		
		ObjectCapsuleComp->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
		ObjectCapsuleComp->SetVisibility(true, true);
		ObjectCapsuleComp->SetHiddenInGame(bDebugCapsuleVisibility);
		ObjectCapsuleComp->ShapeColor = DebugColour;
	}
	
	
	// Apply DataAsset float settings only if floating enabled... 
	if (bEnableFloating)
	{
		// ...and not using custom float settings on the instance.
		if (!bUseCustomFloatSettings)
		{
			// Leaving this property assignment outside the function, since it is only through this codeflow we want to use it.
			bUseCustomFloatSettings = SettingsAsset->bUseCustomFloatSettings;

#if WITH_EDITOR
			// Calling our new retrieval of float settings function 
			RetrieveFloatAssetSettings();
#endif
		}
	}

	// Apply default PointLightComp Settings (if enabled)
	if (ObjectPointLightComp)
	{
		if (!bUseCustomLightSettings)
		{
			bUseCustomLightSettings = SettingsAsset->bUseCustomLightSettings;
			
#if WITH_EDITOR
			// Calling our new retrieval of float settings function 
			RetrieveLightAssetSettings();
#endif
		}

		ObjectPointLightComp->SetVisibility(bIsLightOn);
		ObjectPointLightComp->SetRelativeLocation(ObjectLightLocation);
		
		ObjectPointLightComp->SetIntensity(LightIntensity);
		ObjectPointLightComp->SetAttenuationRadius(LightAttenuationRadius);
		ObjectPointLightComp->SetSourceRadius(LightSourceRadius);
		ObjectPointLightComp->SetLightColor(LightColour);
		ObjectPointLightComp->SetCastShadows(bDoesLightCastShadow);
	}
	return true;
}

// A default function for InteractableItem and PlayerCheckpoint to use, APickup should run PickupItem() instead.
// This function can (and should be) be overridden in subclasses.
void APickup::PlayerEntersInteractable()
{
	if (bDebugEnabled)
	{
		FString objectName = this->GetName();
		check(GEngine != nullptr);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, DebugColour, TEXT("...player enters my InteractableZone. " + objectName));
	}

	// Run the above debug code if debug is enabled when we don't want to change anything.
	if (bIsActivated && bOneTimeUseOnly) return;
	
	// Toggle the light, if enabled on the object
	if (bOverlapTogglesLight)
	{
		bIsLightOn = !bIsLightOn;
		ObjectPointLightComp->SetVisibility(bIsLightOn);
	}
}

#if WITH_EDITOR

// Retrieve the Mesh Settings from the SettingsAsset
void APickup::RetrieveMeshAssetSettings()
{
		// Moving, rotating or scaling the mesh according to settingsAsset.
		ObjectMeshOffset = SettingsAsset->DefaultObjectMeshLocationOffset;
		ObjectMeshRotation = SettingsAsset->DefaultObjectMeshRotation;
		ObjectMeshScale = SettingsAsset->DefaultObjectMeshScale;

	// StaticMeshComponent Settings
	if (SettingsAsset->DefaultMesh.IsValid())
	{
		ObjectMesh = SettingsAsset->DefaultMesh.LoadSynchronous();
		ObjectMeshComp->SetStaticMesh(ObjectMesh.Get());
		
		// Apply the default material
		if (SettingsAsset->DefaultMaterial.IsValid())
		{
			ObjectMaterial = SettingsAsset->DefaultMaterial.LoadSynchronous();
			ObjectMeshComp->SetMaterial(0, ObjectMaterial.Get());
		}
	}
}
	
// Retrieve the Float Settings from the SettingsAsset
void APickup::RetrieveFloatAssetSettings()
{
	// Float Settings:
	FloatAxis = SettingsAsset->DefaultFloatAxis;

	FloatStartPosition = SettingsAsset->DefaultFloatStartPosition;
	CurrentFloatPosition = FloatStartPosition;
		
	bStartInPositiveDirection = SettingsAsset->bStartInPositiveDirection;
	bIsMovingInPositiveDirection = bStartInPositiveDirection;
		
	bUseSineWave = SettingsAsset->bUseSineWave;
		
	FloatingDistance = SettingsAsset->DefaultFloatingDistance;
	FloatingSpeed = SettingsAsset->DefaultFloatingSpeed;
}

// Retrieve the Light Settings from the SettingsAsset
void APickup::RetrieveLightAssetSettings()
{
	// PointLightComponent Settings
	ObjectLightLocation = SettingsAsset->DefaultLightLocation;
	
	LightIntensity = SettingsAsset->DefaultLightIntensity;
	LightAttenuationRadius = SettingsAsset->DefaultAttenuationRadius;
	LightSourceRadius = SettingsAsset->DefaultLightSourceRadius;
	LightColour = SettingsAsset->DefaultLightColour;
	bDoesLightCastShadow = SettingsAsset->bDoesLightCastShadow;
}

// This is just to run debugLines when floating, to see range of float up, and float down. Helps avoid mesh collisions. 
void APickup::OnConstruction(const FTransform& Transform)
{
	// A bit quick-n-dirty fix to ensure mesh gets updated.

	if (ObjectMeshComp)
	{
		if (SettingsAsset)
		{
			// Moving, rotating or scaling the mesh according to settingsAsset.
			ObjectMeshOffset = SettingsAsset->DefaultObjectMeshLocationOffset;
			ObjectMeshRotation = SettingsAsset->DefaultObjectMeshRotation;
			ObjectMeshScale = SettingsAsset->DefaultObjectMeshScale;
		}
		
		ObjectMeshComp->SetRelativeLocation(ObjectMeshOffset);
		ObjectMeshComp->SetRelativeRotation(ObjectMeshRotation);
		ObjectMeshComp->SetRelativeScale3D(ObjectMeshScale);
        
		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Applied mesh transform: Loc=%s Rot=%s Scale=%s"),
			*ObjectMeshOffset.ToString(), *ObjectMeshRotation.ToString(), *ObjectMeshScale.ToString());
		}
	}

	if (ObjectCapsuleComp)
	{
		if (SettingsAsset)
		{
			// Moving, rotating or scaling the mesh according to settingsAsset.
			CapsuleLocationOffset = SettingsAsset->DefaultCapsuleLocation;
			CapsuleRotationOffset = SettingsAsset->DefaultCapsuleRotation;
		}
		
		ObjectCapsuleComp->SetRelativeLocation(CapsuleLocationOffset);
		ObjectCapsuleComp->SetRelativeRotation(CapsuleRotationOffset);
		if (bDebugEnabled)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Applied capsule transform: Loc=%s Rot=%s"),
			*CapsuleLocationOffset.ToString(), *CapsuleRotationOffset.ToString());
		}
	}
	
	
	Super::OnConstruction(Transform);
	
	// Debug has to be enabled, and floating has to be enabled.
	if (!bDebugEnabled || !bEnableFloating) return;
	
	// Draw the floating range (Z axis) in the Editor only
	const FVector FloatRadius = FloatAxis * FloatingDistance;
	const FVector FloatTop = InitialLocation + FloatRadius;
	const FVector FloatBottom = InitialLocation - FloatRadius;

	// Draw the floating range line
    DrawDebugLine(GetWorld(), FloatBottom, FloatTop, DebugColour, false, -1, 0, 2.0f);
    
    // Draw markers at extremes
    DrawDebugSphere(GetWorld(), FloatTop, 10.0f, 8, DebugColour, false, -1, 0, 1.0f);
    DrawDebugSphere(GetWorld(), FloatBottom, 10.0f, 8, DebugColour, false, -1, 0, 1.0f);
    
    // Add labels
    DrawDebugString(GetWorld(), FloatTop + FVector(0, 0, 15), TEXT("Top"), nullptr, DebugColour, -1, true, 1.0f);
    DrawDebugString(GetWorld(), FloatBottom - FVector(0, 0, 25), TEXT("Bottom"), nullptr, DebugColour, -1, true, 1.0f);
    
    // Show the starting position
    FVector StartOffset = FloatAxis * (FloatStartPosition * FloatingDistance);
    const FVector StartPosition = InitialLocation + StartOffset;
    DrawDebugSphere(GetWorld(), StartPosition, 15.0f, 12, FColor::White, false, -1, 0, 1.0f);
    DrawDebugString(GetWorld(), StartPosition + FVector(0, 0, 25), TEXT("Start"), nullptr, FColor::White, -1, true, 1.0f);
    
    // If we have pause duration, show it
    if (FloatPauseDuration > 0.0f)
    {
        FString pauseText = FString::Printf(TEXT("Pause: %.1fs"), FloatPauseDuration);
        DrawDebugString(GetWorld(), FloatTop + FVector(0, 0, 35), pauseText, nullptr, FColor::Yellow, -1, true, 1.0f);
        DrawDebugString(GetWorld(), FloatBottom - FVector(0, 0, 45), pauseText, nullptr, FColor::Yellow, -1, true, 1.0f);
    }
    
    // Visualize path differently for sine wave vs ping-pong
    if (bUseSineWave)
    {
        // For sine wave, show a smooth curve
        const int32 NumPoints = 20;
        FVector PrevPoint = FloatBottom;
        
        for (int32 i = 1; i <= NumPoints; i++)
        {
            float t = (float)i / NumPoints;
            float angle = t * PI; // 0 to π
            
            // Generate a sine wave from -1 to 1
            float sinValue = FMath::Sin(angle);
            
            // Calculate the point on the path
            FVector CurrentPoint = InitialLocation + (FloatAxis * sinValue * FloatingDistance);
            
            // Draw the segment
            DrawDebugLine(GetWorld(), PrevPoint, CurrentPoint, DebugColour, false, -1, 0, 1.0f);
            
            PrevPoint = CurrentPoint;
        }
    }
    else
    {
        // For ping-pong, show a direct line
        DrawDebugDirectionalArrow(GetWorld(), FloatBottom, FloatTop, 10.0f, 
            bStartInPositiveDirection ? FColor::Green : DebugColour, false, -1, 0, 1.0f);
        DrawDebugDirectionalArrow(GetWorld(), FloatTop, FloatBottom, 10.0f, 
            !bStartInPositiveDirection ? FColor::Green : DebugColour, false, -1, 0, 1.0f);
    }
}

/** Mostly for testing purposes early-on, use the AssetSettings created inside Unreal (Game/Settings) instead to change the defaults
 * within all instances of a class. (This might also be needed to change stuff in the Editor while seeing the changes.)
 *
 * Since all our variables are set with UPROPERTY and can be edited in the Editor, this function helps make sure
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
		ObjectCapsuleComp.Get()->GetComponentRotation().Quaternion(), FColor::Red, false, -1, 0, 1.f);
		}
	}

	// Safety check
    if (!PropertyChangedEvent.Property)
        return;
        
    const FName ChangedProperty = PropertyChangedEvent.Property->GetFName();

    // Handle mesh change
    if (ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, ObjectMesh))
    {
        if (!ObjectMeshComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ ObjectMeshComp is null in PostEditChangeProperty! %s"), *GetName());
            return;
        }
        
        if (ObjectMesh.IsValid() || ObjectMesh.ToSoftObjectPath().IsValid())
        {
            UStaticMesh* LoadedMesh = ObjectMesh.LoadSynchronous();
            if (LoadedMesh)
            {
                ObjectMeshComp->SetStaticMesh(LoadedMesh);
                UE_LOG(LogTemp, Warning, TEXT("👍 ObjectMesh loaded in PostEditChangeProperty in %s!"), *GetName());
                
                // Mark render state dirty to force a refresh
                ObjectMeshComp->MarkRenderStateDirty();
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("❌ Failed to load mesh in PostEditChangeProperty! %s"), *GetName());
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ Can't update the static mesh in PostEditChangeProperty! ObjectMesh is null! %s"), *GetName());
        }
    }
    // Handle material change
    else if (ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, ObjectMaterial))
    {
        if (!ObjectMeshComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ ObjectMeshComp is null in PostEditChangeProperty! %s"), *GetName());
            return;
        }
        
        if (ObjectMaterial.IsValid() || ObjectMaterial.ToSoftObjectPath().IsValid())
        {
            UMaterialInterface* LoadedMaterial = ObjectMaterial.LoadSynchronous();
            if (LoadedMaterial)
            {
                ObjectMeshComp->SetMaterial(0, LoadedMaterial);
                UE_LOG(LogTemp, Warning, TEXT("👍 Material updated in PostEditChangeProperty! %s"), *GetName());
                
                // Mark render state dirty to force a refresh
                ObjectMeshComp->MarkRenderStateDirty();
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("❌ Failed to load material in PostEditChangeProperty! %s"), *GetName());
            }
        }
    }
    // Handle mesh transform properties
    else if (ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, ObjectMeshOffset) ||
             ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, ObjectMeshRotation) ||
             ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, ObjectMeshScale))
    {
        if (!ObjectMeshComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ ObjectMeshComp is null in PostEditChangeProperty! %s"), *GetName());
            return;
        }
        
        // Apply all transform components to ensure consistency
        ObjectMeshComp->SetRelativeLocation(ObjectMeshOffset);
        ObjectMeshComp->SetRelativeRotation(ObjectMeshRotation);
        ObjectMeshComp->SetRelativeScale3D(ObjectMeshScale);
        
        UE_LOG(LogTemp, Verbose, TEXT("Updated mesh transform in PostEditChangeProperty! %s"), *GetName());
    }
	
	
	// This isn't pretty, but Unreal has no native in-built way to check fields inside components.
	// Check if any field in ObjectCapsuleComp has changed.
	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, CapsuleRadius) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, CapsuleHalfHeight) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, CapsuleLocationOffset) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, CapsuleRotationOffset) ||
		ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, bDebugCapsuleVisibility))
	{
		// Check if CapsuleComp not nullptr
		if (ObjectCapsuleComp)
		{
			// TODO: Fix weird issues - the editor does not like it when we run the radius and half-height code like this.
			
			// Enforce the capsule size constraints - to avoid issues with possible bugs / crashes regarding capsules.
			// Is a bit ... weird, can occasionally lead to freezes / crashes, due to constraits.
			if (CapsuleHalfHeight <= CapsuleRadius)
			{
				CapsuleHalfHeight = CapsuleRadius;
			}
			else if (CapsuleRadius >= CapsuleHalfHeight)
			{
				CapsuleRadius = CapsuleHalfHeight;
			}
			
			ObjectCapsuleComp->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
			ObjectCapsuleComp->SetRelativeLocation(CapsuleLocationOffset);
			ObjectCapsuleComp->SetRelativeRotation(CapsuleRotationOffset);

			// Toggling seeing the capsule in Playmode.
			ObjectCapsuleComp->SetHiddenInGame(bDebugCapsuleVisibility);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ ObjectCapsuleComp is null in PostEditChangeProperty! %s"), *GetName())
		}
	}

	// Normalize FloatAxis if it's modified
	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(APickup, FloatAxis))
	{
		// Ensure the axis vector is not zero
		if (FloatAxis.IsNearlyZero())
		{
			FloatAxis = FVector(0.f, 0.f, 1.f);
			UE_LOG(LogTemp, Warning, TEXT("Float axis cannot be zero. Reset to default (0,0,1)"));
		}
		else
		{
			// Normalize to make sure it's a unit vector
			FloatAxis.Normalize();
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


// Mostly just for debugging purposes, since we are destroying the objects from the player BP instead.
void APickup::PickupItem()
{
	if (bDebugEnabled)
	{
		FString objectName = this->GetName();
		check(GEngine != nullptr);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, DebugColour,TEXT("I'm being picked up! ... remember me as " + objectName));
	}
}
#endif

void APickup::BeginPlay()
{
	Super::BeginPlay();
    
	// Store the initial location
	InitialLocation = GetActorLocation();
    
	// Initialize floating variables
	if (bEnableFloating)
	{
		// Set the initial CurrentFloatPosition based on FloatStartPosition
		CurrentFloatPosition = FloatStartPosition;
        
		// Set the initial direction
		bIsMovingInPositiveDirection = bStartInPositiveDirection;
        
		// Apply initial position for ping-pong movement (sine wave position is calculated continuously)
		if (!bUseSineWave)
		{
			const float Offset = CurrentFloatPosition * FloatingDistance;
			const FVector OffsetVector = FloatAxis * Offset;
			SetActorLocation(InitialLocation + OffsetVector);
		}
        
		// Reset pause state
		bFloatIsPaused = false;
		StoredFloatPausedTime = 0.0f;
	}
}

void APickup::UnpauseMovement()
{
	// First clear pause state
	bFloatIsPaused = false;
    
	// Toggle movement direction
	bIsMovingInPositiveDirection = !bIsMovingInPositiveDirection;
    
	// Clear any existing timers
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(FloatPauseTimerHandle);
	}
    
	// Log if debugging
	if (bDebugEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("Platform %s resumed moving, direction: %s"), 
			*GetName(), 
			bIsMovingInPositiveDirection ? TEXT("positive") : TEXT("negative"));
	}

}

// The function for floating. Used by Pickup, and MovingPlatform (+ possibly Hazards). To be overridden.
// 1. Run a check if paused, and returns the function after incrementing on the pausetimer.
// 2. If not paused, check and run respective desired float (ping-pong or sine wave).
void APickup::Float()
{
	// Check if floating is paused
    if (bFloatIsPaused)
    {
	    // For infinite pauses, just return immediately
    	/*if (FloatPauseDuration <= 0.0f)
		{
			return;
		}*/
    	
    	// Only set up the timer ONCE when we first pause, not every frame
    	if (!GetWorld()->GetTimerManager().IsTimerActive(FloatPauseTimerHandle))
    	{
    		// Clear any existing timers first to be safe
    		GetWorld()->GetTimerManager().ClearTimer(FloatPauseTimerHandle);
    
    		// Set up a direct function pointer timer (simple and efficient)
    		GetWorld()->GetTimerManager().SetTimer(
				FloatPauseTimerHandle,
				this,  // Object to call the function on
				&APickup::UnpauseMovement,  // Direct function pointer
				FloatPauseDuration,  // How long to wait
				false  // Don't loop the timer
			);
    
    		// Add debug logging
    		if (bDebugEnabled)
    		{
    			UE_LOG(LogTemp, Warning, TEXT("%s paused at %s, timer set for %.2f seconds"), 
					*GetName(), 
					*GetActorLocation().ToString(),
					FloatPauseDuration);
    		}
    	}
    }
	
    // Get the current time
    float Time = GetWorld()->TimeSeconds;
    
    if (bUseSineWave)
    {
        // Run it in a Sin function, which takes an angle in radians and returns a value of -1 to 1.
        // Speed of the sin-wave is time (multiplied with FloatingSpeed), and amplitude (height) is set by FloatingDistance.
        float Offset = FMath::Sin(Time * FloatingSpeed) * FloatingDistance;

        // Offset fluctuates between -FloatingDistance and +FloatingDistance, so we float.
        // Using InitialLocation to remain where the object is spawned.
        const FVector OffsetVector = FloatAxis * Offset;
        SetActorLocation(InitialLocation + OffsetVector);
        
        // Additional code to detect when the sine wave is near the extremes
        // and optionally pause at those points
        if (FloatPauseDuration > 0.0f)
        {
            // Calculate current position in the cycle (0 to 2π), by using modulus 
            float CyclePosition = FMath::Fmod(Time * FloatingSpeed, 2.0f * PI);
            
            // Check if we're near a peak (π/2) or trough (3π/2)
            // The threshold determines how close to the exact peak/trough we need to be
            const float threshold = 0.05f;

        	// Trigonometry at play - When between -90° (PI*1.5) to 90° (PI*0.5), sin = -1 || sin = 1, we're
        	// close enough to reach sin values at the extremes, where the float movement slows down at the top / bottom.
            if (FMath::Abs(CyclePosition - (PI * 0.5f)) < threshold || 
                FMath::Abs(CyclePosition - (PI * 1.5f)) < threshold)
            {
                // We're at an extreme point - pause the movement
                bFloatIsPaused = true;
            }
        }
    }
    else // Ping-pong movement
    {
        // Using (frame) DeltaTime for more reliable movement
        const float DeltaTime = GetWorld()->GetDeltaSeconds();
        
        // Calculate position change based on FloatingSpeed (units per second)
        // The full range is from -1.0 to 1.0, or 2.0 total
        // So we divide by (FloatingDistance * 2) to get the rate per unit distance
        const float MovementRate = (FloatingSpeed / (FloatingDistance * 2.0f)) * DeltaTime;
        
        // Update position based on the direction
        if (bIsMovingInPositiveDirection)
        {
            CurrentFloatPosition += MovementRate;
            if (CurrentFloatPosition > 1.0f)
            {
                CurrentFloatPosition = 1.0f; // Cap at exactly 1.0
                
                // Set paused state if we have a pause duration
                if (FloatPauseDuration > 0.0f)
                {
                    bFloatIsPaused = true;
                }
                else
                {
                    // Toggle direction immediately if no pause
                    bIsMovingInPositiveDirection = false;
                }
            }
        }
        else
        {
            CurrentFloatPosition -= MovementRate;
            if (CurrentFloatPosition < -1.0f)
            {
                CurrentFloatPosition = -1.0f; // Cap at exactly -1.0
                
                // Set paused state if we have a pause duration
                if (FloatPauseDuration > 0.0f)
                {
                    bFloatIsPaused = true;
                }
                else
                {
                    // Toggle direction immediately if no pause
                    bIsMovingInPositiveDirection = true;
                }
            }
        }
        
        // Calculate offset based on CurrentFloatPosition
        const float Offset = CurrentFloatPosition * FloatingDistance;
        
        // Apply offset along the chosen axis
        const FVector OffsetVector = FloatAxis * Offset;
    	SetActorLocation(InitialLocation + OffsetVector);
    }
}


void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Floating needs to be enabled and the object needs to be activated
	if (bEnableFloating && bIsActivated)
	{
		Float();
	}
}

// This was the built-in functionality for items when interacting with the Guardian, but due to HUD code and overall
// how it ended up being designed in BP for the GuardianCharacter. This code isn't used in Pickup but for derived subclasses.
// This delegate function gets bound inside AWorldObject::BeginPlay(), and subclasses need to override this and GetAllOverlapComponents().
void APickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if it's ACharacter (since we have the player and EnemyAI)
	if (const ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		// Check if this character is controlled by a player (eliminate enemy AI from the logic)
		if (Cast<APlayerController>(Character->GetController()))
		{
			// InteractableItem and PlayerCheckpoint should always have the bool bIsInteractable enabled.
			if (bIsInteractable)
			{
				PlayerEntersInteractable();
			}
			else
			{
#if WITH_EDITOR
				PickupItem();
#endif
			}
		}
	}
}

// Add any overlapComponents in your class but ensure you do not add your component more than once.
// Since it is a virtual override, you just insert every component your class currently uses.
TArray<UPrimitiveComponent*> APickup::GetAllOverlapComponents() const
{
	TArray<UPrimitiveComponent*> OverlapComponents;
	OverlapComponents.Add(ObjectCapsuleComp.Get());
	return OverlapComponents;
}
