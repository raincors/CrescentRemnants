// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppDoxygenUnresolvedReference
#include "WorldObject.h"
#include "Components/SceneComponent.h"

/** Worth knowing is that any class deriving off of AWorldObject runs its constructor to initialise components and apply
 * a default set of variables. The C++ implementation of doing things; the back-up plan, or Plan B.
 *
 * All components initialised here in the constructor will be inherited and already initialised for subclasses.
 * Inherited classes should only add new components or modify the variables of existing components.
 */
AWorldObject::AWorldObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create and set the root scene component
	ObjectRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	// REMEMBER THIS. Seriously. It is necessary for all AActors.
	RootComponent = ObjectRoot;

	// Default constructor values, usually overridden in UseWorldObjectAssetSettings()
	ObjectRoot->SetWorldRotation(RootObjectRotation);
	ObjectRoot->SetWorldScale3D(RootObjectScale);
}

/** Settings applied inside UseWorldObjectAssetSettings() from the SettingsAsset (UDataAsset):
 *
 * This function pulls values from the asset and applies them to the world object.
 * It sets any object flags, override permissions, visual debug toggles, and root transformation properties.
 * 
 * @param bAllowSettingsOverride	- Allow instances to override the asset settings?
 * @param bOneTimeUseOnly			- Does this object only end up being activated / used once?
 * @param bIsActivated				- Is this object active (or active after being constructed)?
 * 
 * @param bDebugEnabled				- Are debug visuals and text enabled in Playmode?
 * @param DebugColor				- What DebugColor do you want for your object?
 *
 * @param RootObjectRotation		- Rotation to apply to the Root component.
 * @param RootObjectScale			- Scale to apply to the Root component.
 */
bool AWorldObject::UseWorldObjectAssetSettings()
{
	// if we can't find the dataAsset, then we're not using Asset Settings.
	if (!SettingsAsset.IsValid())
	{
		return false;
	}
	// And if the dataAsset has toggled that we are not using the DataAsset, we do not use it.
	if (!SettingsAsset->bApplyDefaultAssetSettings)
	{
		return false;
	}
	
	// Allow instances to override?
	bAllowSettingsOverride = SettingsAsset->bAllowInstancesToOverride;
	// Is the object a one-time use only?
	bOneTimeUseOnly = SettingsAsset->bOneTimeUse;
	// Is the object activated? To be used by subclasses; checkpoints, pickups (floating), platforms, etc.
	bIsActivated = SettingsAsset->bIsActivated;
	
	// Enable Debug visuals in Playmode?
	bDebugEnabled = SettingsAsset->bEnableDebug;
	// What debug colour do you want for instances of this specific class?
	DebugColour = SettingsAsset->DefaultDebugTextColour;
	
	// Assigning variables from AssetSettings to SceneComponent (Root):
	RootObjectRotation = SettingsAsset->DefaultObjectRotation;
	RootObjectScale = SettingsAsset->DefaultObjectScale;
	
	// Applying the new Root Rotation and Scale
	ObjectRoot->SetWorldRotation(SettingsAsset->DefaultObjectRotation);
	ObjectRoot->SetWorldScale3D(SettingsAsset->DefaultObjectScale);

	return true;
}

#if	WITH_EDITOR
// Called when an object is moved or updated in the Editor
void AWorldObject::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Just for the base class - to avoid reloading the settings everytime OnConstruction runs. We only want to run it once.
	if (!bAttemptedRetrievalOfSettings)
	{
		bAttemptedRetrievalOfSettings = true;
		
		// Using TSoftObjectPtr, we can load the AssetSettings more safely, which doesn't crash Unreal on start-up if there is an error.
		// We're also using the virtual getter GetDefaultSettingAssetPath() that helps retrieve any paths to DefaultSettings.
		TSoftObjectPtr<UWorldObjectSettings> WorldObjectSettings = TSoftObjectPtr<UWorldObjectSettings>
		(FSoftObjectPath(GetDefaultSettingAssetPath()));

		// Have we found the asset?
		if (WorldObjectSettings.IsValid())
		{
			SettingsAsset = WorldObjectSettings.Get();
			if (UseWorldObjectAssetSettings())
			{
				UE_LOG(LogTemp, Warning, TEXT("👍 Using Asset Settings in %s!"), *GetClass()->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("❌ NOT using Asset Settings in %s!"), *GetClass()->GetName());
			}
		}
		else
		{
			// Try loading the SettingsAsset, and this works, assign it. If not, run a log warning.
			if (TSoftObjectPtr<UWorldObjectSettings> LoadedSettings = WorldObjectSettings.LoadSynchronous())
			{
				SettingsAsset = LoadedSettings.Get();
				if (UseWorldObjectAssetSettings())
				{
					UE_LOG(LogTemp, Warning, TEXT("👍 Using Asset Settings in %s!"), *GetClass()->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("❌ NOT using Asset Settings in %s!"), *GetClass()->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("❌ Could not load ObjectAssetSettings in %s!"), *GetClass()->GetName());
			}
		}
	}
}
#endif

/** Called when the game starts or when spawned. Stores the InitialLocation in-game for all actors inheriting from WorldObject.
 *
 * @param InitialLocation - InitialActorLocation when the game starts.
 */
void AWorldObject::BeginPlay()
{
	Super::BeginPlay();

	// Whenever the object is in during PlayMode, we get the initial location of the object and store it. 
	InitialLocation = GetActorLocation();

	// Going through an array of possible components you want playerOverlap events on, whether it's one or multiple,
	// thanks to the TArray<UPrimitiveComponent*> virtual getter function GetAllOverlapComponents.
	for (UPrimitiveComponent* OverlapComponent : GetAllOverlapComponents())
	{
		if (OverlapComponent)
		{
			OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AWorldObject::OnBeginOverlap);
			OverlapComponent->OnComponentEndOverlap.AddDynamic(this, &AWorldObject::OnEndOverlap);
		}
	}
}

/** Early prototype of overlap handling using delegates inside Unreal, before I realised you could use NotifyBeginOverlap().
 * This is still used instead of NotifyBeginOverlap(), since I had already implemented this system.
 *
 * Our custom OnBeginOverlap is using a built-in delegate from UPrimitiveComponent "->OnComponentBeginOverlap",
 * requires 6 parameters, but we only really use AActor* OtherActor.
 *
 * Every Collision Component needs to have this delegate bound inside BeginPlay() in WorldObject.cpp, via .AddDynamic.
 *
 * UComponent->OnComponentBeginOverlap.AddDynamic.
 */
void AWorldObject::OnBeginOverlap(UPrimitiveComponent*, AActor* OtherActor,
                    UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	// The subclasses override what happens when the player begins to overlap. :)
}

/**	Early prototype of overlap handling using delegates inside Unreal, before I realised you could use NotifyEndOverlap().
 * This is still used instead of NotifyEndOverlap(), since I had already implemented this system.
 *
 * Our custom OnEndOverlap is using a built-in delegate from UPrimitiveComponent "->OnComponentEndOverlap",
 * requires 4 parameters, but we only really use AActor* OtherActor.
 *
 * Every Collision Component needs to have this delegate bound inside BeginPlay() in WorldObject.cpp, via .AddDynamic.
 * 
 * UComponent->OnComponentEndOverlap.AddDynamic.
 */
void AWorldObject::OnEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
	// The subclasses override what happens when the player ends overlap. :)
}

