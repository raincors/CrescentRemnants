// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CRESCENTREMNANTS_API AMovingPlatform : public APickup
{
	GENERATED_BODY()

public:

	// Constructors should be public in Unreal Engine. Otherwise, you will have trouble spawning your object.
	AMovingPlatform();

protected:

	UPROPERTY(EditAnywhere, Category = "Movement|Platform", meta = (AllowPrivateAccess = "true"))
	bool bIsBreakable = false;

	UPROPERTY(EditAnywhere, Category = "Movement|Platform", meta = (AllowPrivateAccess = "true"))
	bool bMoveWhenPlayerOn = true;
	
	virtual void UnpauseMovement() override final;

	// Custom function - Using UWorldObject settings for this class and adding additional Pickup features.
	virtual bool UseWorldObjectAssetSettings() override final;

	// Custom function for floating, which subclasses can override.
	virtual void Float() override final;

public:

	// We want the platforms to collide with anything, and the platforms are moving, so WorldDynamic seems the best fit.
	virtual FName GetMeshCollisionTag() const override final{ return TEXT("WorldDynamic"); }
	
	// DataAssetPath for subclasses to override with their own DataAsset path
	virtual FString GetDefaultSettingAssetPath() const override final
	{ return TEXT("/Game/Settings/MovingPlatform_Default.MovingPlatform_Default"); }

	// UStaticMesh - Get the default ObjectMesh; subclasses override and swap with own mesh.
	virtual TSoftObjectPtr<UStaticMesh> GetObjectStaticMesh() const override final
	{ return ObjectMesh.IsValid() ? ObjectMesh.Get() : Super::GetObjectStaticMesh(); }

	// UMaterialInterface - Get the default Material / MaterialInstance; subclasses override and swap with own material.
	virtual TSoftObjectPtr<UMaterialInterface> GetObjectMaterial() const override final
	{ return ObjectMaterial.IsValid() ? ObjectMaterial.Get() : Super::GetObjectMaterial(); }
};
