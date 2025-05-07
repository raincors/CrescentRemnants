// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldObjectSettings.h"  
#include "WorldObject.generated.h"

/** This is an abstract class and is just used as a template for the subclasses to derive components from.
 * Overall, to improve readability and not have to write the same code everywhere in a lot of classes.
 *
 * It uses the SettingsAsset (UWorldObjectSettings: public UDataAsset) to apply variables and settings from the Editor
 * to the classes in OnConstruction(); after the constructor has initialised components. Plan A for all classes.
 *
 * Keep in mind all constructors use default instance variables as Plan B if Plan A doesn't work.
 *
 * WorldObject Components:
 * - SettingsAsset (UWorldObjectSettings)		- Derived from UDataAsset, with asset settings for each class.
 * - Root (USceneComponent)						- Scene- / ObjectRoot of the actor
 */
UCLASS(Abstract, NotBlueprintable, HideCategories=(Input, Actor, ActorTick))
class CRESCENTREMNANTS_API AWorldObject : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AWorldObject();

	// Custom function - Use the DataAsset settings instead of constructor defaults
	virtual bool UseWorldObjectAssetSettings();

	// Shared settings asset for all world objects (to be overridden in subclasses) - public for WorldObjectSettings.cpp
	UPROPERTY(BlueprintReadWrite, Category = "Component")
	TSoftObjectPtr<UWorldObjectSettings> SettingsAsset;

protected:
	
	// Scene Root Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USceneComponent> ObjectRoot;

	// Have we retrieved the Settings DataAsset? Transient makes each instance start with it at false.
	UPROPERTY(Transient)
	bool bAttemptedRetrievalOfSettings;

	/** --- Properties / Variables ---
	 * Categories (in order):
	 * - Debug
	 * - Object Bool
	 * - Transform
	 */
	
	// Debug - Enable Debugging for the instances of the class?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override | Settings", meta = (AllowPrivateAccess = "true"))
	bool bDebugEnabled = false;

	// Debug - What colour should instances of this class have when printing text / drawing debug lines?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override | Settings", meta = (AllowPrivateAccess = "true"))
	FColor DebugColour = FColor::White;

	// Object Bool - For when the object is activated; floating, checkpoint active, platform moving, etc.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override | Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsActivated = true;

	// Object Bool - Set true ONLY for objects that should be usable once (checkpoints, collectibles)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override | Settings", meta = (AllowPrivateAccess = "true"))
	bool bOneTimeUseOnly = false;
	
	// Transform - Initial location this object instance is in.
	FVector InitialLocation = FVector(0, 0, 0);

	// Transform - Object Rotation for the object instance.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Transform", meta = (AllowPrivateAccess = "true"))
	FRotator RootObjectRotation = FRotator(0.f, 0.f, 0.f);

	// Transform - Object Scale for the object instance.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Transform", meta = (AllowPrivateAccess = "true"))
	FVector RootObjectScale = FVector(1.f, 1.f, 1.f);
	
#if	WITH_EDITOR
	/** Runs in the Editor whenever a property or its transform is modified -> Very often! Be aware of this.
	* Also runs once in PlayMode before BeginPlay().
	* 
	* Using a flag (bAttemptedRetrievalOfSettings), we retrieve AssetSettings once when the object is created.
	*/
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
	
	// We bind delegates here, with OnBeginOverlap and OnEndOverlap. Also runs InitialLocation.
	virtual void BeginPlay() override;

	// For subclasses - when an object starts overlapping with this object
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
										 bool bFromSweep, const FHitResult& SweepResult);

	// For subclasses - when an object stops overlapping with this object
	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
									   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	
	// Virtual getters for classes to override the returns with their own instanced variables, components, etc.

	// Mesh Collision Tag - For subclasses to inherit or override. Default = TEXT("NoCollision").
	virtual FName GetMeshCollisionTag() const { return TEXT("NoCollision"); }

	// DataAssetPath for subclasses to override with their own DataAsset path - using the Pickup_Default as base default.
	virtual FString GetDefaultSettingAssetPath() const
	{ return TEXT("/Game/Settings/Pickup_Default.Pickup_Default"); }

	// Subclasses override this to assign delegates to OnBeginOverlap or OnEndOverlap with .AddDynamic.
	// C++ will "go down" the inheritance chain to find the most derived virtual override and use that.
	virtual TArray<UPrimitiveComponent*> GetAllOverlapComponents() const { return {}; }
	
};
