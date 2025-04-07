// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldObjectSettings.h"  
#include "WorldObject.generated.h"

/** This is an abstract class, and is just used as a template for the subclasses to derive components from.
 * Overall to improve readability and not have to write the same code everywhere in a lot of classes.
 *
 * It uses the SettingsAsset (UWorldObjectSettings : UDataAsset) to apply variables and settings from the Editor to
 * the classes in OnConstruction(); after the constructor has initialized components. Plan A for all classes.
 *
 * Keep in mind all constructors use default instance variables as Plan B, if Plan A doesn't work.
 *
 * WorldObject Components:
 * - SettingsAsset (UWorldObjectSettings)		- UDataAsset, with asset settings for each class.
 * - Root (USceneComponent)						- Root of the actor
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TSoftObjectPtr<UWorldObjectSettings> SettingsAsset;

protected:
	
	// Scene Root Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USceneComponent> ObjectRoot;

	// Have we tried to retrieve the Settings DataAsset? Transient to make each instance start with it at false.
	UPROPERTY(Transient)
	bool bAttemptedRetrievalOfSettings = false;

	// For when checkpoint is activated
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true"))
	bool bIsActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true"))
	bool bOneTimeUseOnly = false;
	
	// Debug - Enable Debugging?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true"))
	bool bDebugEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true"))
	FColor DebugColour = FColor::White;

	// Can instances override settings from the DataAsset?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true"))
	bool bAllowSettingsOverride = false;
	
	// Initial location this object instance is in.
	FVector InitialLocation = FVector(0, 0, 0);

	// Object Rotation for the object instance.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Transform", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FRotator RootObjectRotation = FRotator(0.f, 0.f, 0.f);

	// Object Scale for the object instance.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Transform", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FVector RootObjectScale = FVector(1.f, 1.f, 1.f);
	
	/** Runs in Editor whenever a property or its transform is modified -> Very often! Be aware of this.
	* Also runs once in PlayMode before BeginPlay().
	* 
	* Using a flag (bAttemptedRetrievalOfSettings) we retrieve AssetSettings once only, when the object is created.
	*/
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// We bind delegates here, with OnBeginOverlap and OnEndOverlap. Also runs InitialLocation.
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
	// Runs in the Editor only, when you modify a property on an object
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// For subclasses - when player starts overlapping with the object collision
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
										 bool bFromSweep, const FHitResult& SweepResult);

	// For subclasses - when player stops overlapping with the object collision
	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
									   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// --- Getters ---

	// Mesh Collision Tag
	virtual FName GetMeshCollisionTag() const { return TEXT("NoCollision"); }

	// DataAssetPath for subclasses to override with their own DataAsset path - using the Pickup_default as base default.
	virtual FString GetDefaultSettingAssetPath() const
	{ return TEXT("/Game/Settings/Pickup_Default.Pickup_Default"); }

	// Subclasses override this to assign delegates to OnBeginOverlap or OnEndOverlap with .AddDynamic.
	// C++ will "go down" the inheritance chain to find the most derived virtual override, and use that.
	virtual TArray<UPrimitiveComponent*> GetAllOverlapComponents() const { return {}; }
	
};
