// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "PlayerCheckpoint.generated.h"

/**
 * What functionality on-top of this does PlayerCheckpoint need?
 * - Override playerEnterOverlap with bIsActivated instead of Destroy().
 * - Send information about number of activated checkpoints to a GameManager e.g. to track available checkpoints
 * - If the only active checkpoint, respawn player during PlayerDeath.
 * - Else... check distance to player, sent to e.g. a GameManager to find which checkpoint is closest, and then respawn player. 
 */

UCLASS()
class CRESCENTREMNANTS_API APlayerCheckpoint : public APickup
{
	GENERATED_BODY()
	
public:
	APlayerCheckpoint();

protected:
	
	// Custom function - Using UWorldObject settings for this class?
	virtual bool UseWorldObjectAssetSettings() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Custom function for floating
	virtual void Float() override;

	// Not really used, but for derived objects to be able to use it.
	virtual void Tick(float DeltaTime) override;
	
	// Run the right variables in here to be able to change stuff directly in Editor and see the dynamic changes
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// Runs once, as soon as the player enters overlap range with the object.
	virtual void PlayerEntersInteractable() override;

	// You can read more about this function in WorldObject.h. Should not be UFUNCTION Here, since WorldObject does it.
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	// --- Getters ---

	// DataAssetPath for subclasses to override with their own DataAsset path
	virtual FString GetDefaultSettingAssetPath() const override
	{ return TEXT("/Game/Settings/PlayerCheckpoint_Default.PlayerCheckpoint_Default"); }

	// UStaticMesh - Get the default ObjectMesh; subclasses override and swap with own mesh.
	virtual TSoftObjectPtr<UStaticMesh> GetObjectStaticMesh() const override
	{ return ObjectMesh.IsValid() ? ObjectMesh.Get() : Super::GetObjectStaticMesh(); }

	// UMaterialInterface - Get the default Material / MaterialInstance; subclasses override and swap with own material.
	virtual TSoftObjectPtr<UMaterialInterface> GetObjectMaterial() const override
	{ return ObjectMaterial.IsValid() ? ObjectMaterial.Get() : Super::GetObjectMaterial(); }

	// USoundBase - Get the default SoundBase; subclasses override and swap with own sound.
	virtual TSoftObjectPtr<USoundBase> GetObjectSound() const override
	{ return ObjectSound.IsValid() ? ObjectSound.Get() : Super::GetObjectSound(); }

	// Subclasses override this to assign delegates to OnBeginOverlap or OnEndOverlap with .AddDynamic.
	// Since virtual functions only run the most derived version, you have to include all your colliders in every subclass.
	// virtual TArray<UPrimitiveComponent*> GetAllOverlapComponents() const override { return { yourColliderHere.Get() }; }
	
};
