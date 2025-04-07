// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppDoxygenUnresolvedReference
#include "WorldObject.h"
#include "Components/SceneComponent.h"

/** Worth knowing is that any class deriving off of AWorldObject, runs its constructor to initialize components and apply
 * a default set of variables. The C++ implementation of doing things; the back-up plan, or Plan B.
 *
 * All components initialized here in the constructor, will be inherited and already initialized for subclasses.
 * Inherited classes should only add new components or modify the variables of existing components.
 */
AWorldObject::AWorldObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create and set the root scene component
	ObjectRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	// DON'T FORGET THIS. Seriously. It's needed for all AActors.
	RootComponent = ObjectRoot;

	// Default constructor values, usually overridden in UseWorldObjectAssetSettings()
	ObjectRoot->SetWorldRotation(RootObjectRotation);
	ObjectRoot->SetWorldScale3D(RootObjectScale);
}

/** Settings applied here from the SettingsAsset (UDataAsset):
 *
 * This function pulls values from the asset and applies them to the world object.
 * It sets visual debug toggles, override permissions, and root transformation properties.
 * 
 * @param bDebugEnabled	- Are debug visuals and text enabled in Playmode?
 * @param DebugColor				- What DebugColor do you want for your object?
 * @param bAllowSettingsOverride	- Allow instances to override the asset settings?
 * @param RootObjectRotation		- Rotation to apply to the Root component.
 * @param RootObjectScale			- Scale to apply to the Root component.
 */
bool AWorldObject::UseWorldObjectAssetSettings()
{
	if (!SettingsAsset.IsValid())
	{
		return false;
	}
	
	// Enable Debug visuals & text in Playmode?
	bDebugEnabled = SettingsAsset->bEnableDebug;
	DebugColour = SettingsAsset->DefaultDebugTextColour;

	// Allow instances to override?
	bAllowSettingsOverride = SettingsAsset->bAllowInstancesToOverride;
	// Is the object a one-time use only?
	bOneTimeUseOnly = SettingsAsset->bOneTimeUse;
	
	// Assigning variables from AssetSettings to SceneComponent (Root):
	RootObjectRotation = SettingsAsset->DefaultObjectRotation;
	RootObjectScale = SettingsAsset->DefaultObjectScale;
	
	// Applying the new Root Rotation and Scale
	ObjectRoot->SetWorldRotation(SettingsAsset->DefaultObjectRotation);
	ObjectRoot->SetWorldScale3D(SettingsAsset->DefaultObjectScale);

	return true;
}

// Called when an object is moved or updated in the Editor
void AWorldObject::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Just for the base class - to avoid reloading the settings everytime OnConstruction runs. We only want to run it once.
	if (!bAttemptedRetrievalOfSettings)
	{
		bAttemptedRetrievalOfSettings = true;
		
		// Using TSoftObjectPtr, we can load the AssetSettings more safely, which doesn't crash Unreal on start-up if there is an error.
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
			// Try load the SettingsAsset, and this works, assign it. If not, run a log warning.
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
		}
	}
}

/** Called when the game starts or when spawned. Stores the InitialLocation in-game for all actors.
 *
 * @param InitialLocation	InitialActorLocation when the game starts.
 */
void AWorldObject::BeginPlay()
{
	Super::BeginPlay();

	// Whenever the object is in during PlayMode, we get the initial location of the object and store it. 
	InitialLocation = GetActorLocation();

	// Going through an array of possible components you want player overlap events on, whether it's one or multiple,
	// thanks to the TArray<UPrimitiveComponent*> virtual getter function GetAllOverlapComponents.
	for (UPrimitiveComponent* Comp : GetAllOverlapComponents())
	{
		if (Comp)
		{
			// TODO: Might want to add a check and see if a component has already been bound.
			
			Comp->OnComponentBeginOverlap.AddDynamic(this, &AWorldObject::OnBeginOverlap);
			Comp->OnComponentEndOverlap.AddDynamic(this, &AWorldObject::OnEndOverlap);
		}
	}
}

// Called every frame
void AWorldObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

#if WITH_EDITOR

// Called when an object has a field, variable or component modified in the Editor
void AWorldObject::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

/** Our custom OnBeginOverlap is utilizing a built-in delegate from UPrimitiveComponent "->OnComponentBeginOverlap",
 * requires 6 parameters, but we only really use AActor* OtherActor.
 *
 * Every Collision Component needs to bind this delegate inside BeginPlay(), via:
 *
 * UComponent->OnComponentBeginOverlap.AddDynamic.
 */
void AWorldObject::OnBeginOverlap(UPrimitiveComponent*, AActor* OtherActor,
                    UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	// The subclasses override what happens when player begins to overlap. :)
}

/** Our custom OnEndOverlap is utilizing a built-in delegate from UPrimitiveComponent "->OnComponentEndOverlap",
 * requires 4 parameters, but we only really use AActor* OtherActor.
 *
 * Every Collision Component needs to bind this delegate inside BeginPlay(), via .AddDynamic.
 * 
 * UComponent->OnComponentEndOverlap.AddDynamic.
 */
void AWorldObject::OnEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
	// The subclasses override what happens when the player ends overlap. :)
}

