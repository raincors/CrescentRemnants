// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "PlayerCheckpoint.generated.h"

/** PlayerCheckpoint:
 * Modifies inherited properties with own Mesh, Material, CapsuleComponent, etc.
 * Checkpoint last touched has its location stored inside GuardianCharacter as respawnLocation during PlayerDeath().
 */
UCLASS()
class CRESCENTREMNANTS_API APlayerCheckpoint : public APickup
{
	GENERATED_BODY()
	
public:
	APlayerCheckpoint();

protected:

	// Runs once, as soon as the player enters overlap range with the object.
	virtual void PlayerEntersInteractable() override;

	// When this object stops overlapping with another object. You can read more about it in WorldObject.cpp & Pickup.cpp. 
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

public:
	
	// --- Getters ---

	// DataAssetPath for subclasses to override with their own DataAsset path
	virtual FString GetDefaultSettingAssetPath() const override final
	{ return TEXT("/Game/Settings/PlayerCheckpoint_Default.PlayerCheckpoint_Default"); }

	// UStaticMesh - Get the default ObjectMesh; subclasses override and swap with own mesh.
	virtual TSoftObjectPtr<UStaticMesh> GetObjectStaticMesh() const override final
	{ return ObjectMesh.IsValid() ? ObjectMesh.Get() : Super::GetObjectStaticMesh(); }

	// UMaterialInterface - Get the default Material / MaterialInstance; subclasses override and swap with own material.
	virtual TSoftObjectPtr<UMaterialInterface> GetObjectMaterial() const override final
	{ return ObjectMaterial.IsValid() ? ObjectMaterial.Get() : Super::GetObjectMaterial(); }
};
