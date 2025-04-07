// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "InteractableItem.generated.h"

/**
 * What functionality on-top of this does Interactable (e.g. MemoryFragments / Tutorial Bird) need?
 * - Override playerEnterOverlap with WaitingForInteract() - run UI, VFX, etc., waiting for Interact() from the player.
 * - 
 */

class USphereComponent;

UCLASS()
class CRESCENTREMNANTS_API AInteractableItem : public APickup
{
	GENERATED_BODY()

	// A SphereComponent with a bigger radius to help run e.g. animations or visuals when the player is nearby.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Interactable", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> InteractableOverlapSphere;

	/** --- Properties / Variables ---
	 * Categories (in order):
	 * - Bools
	 * - ObjectMesh
	 * - CapsuleComponent
	 * - PointLightComponent
	 * - Float Settings
	 * - Debug Settings
	 */
	
	// Toggle whether the object destroys itself on interact (editable from Details panel, thanks to EditAnywhere)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Interactable", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	bool bDestroyOnInteract = false;

	// Toggle whether the light should turn on/off when interacted with
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Interactable", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	bool bInteractionTogglesLight = false;

	// Is the overlap sphere visible in PlayMode?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Interactable", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	bool bDebugIsOverlapSphereVisible = false;
	
	// Interaction SphereOverlapRadius (detects when a player is nearby)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Interactable", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float InteractableOverlapSphereRadius = 300.f;

	// Interactable CapsuleHalfHeight (Must always be higher or equal to CapsuleRadius)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Interactable", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float InteractableCapsuleHalfHeight = 350.f;
	
	// Interaction CapsuleRadius (Must always be lower or equal to CapsuleHalfHeight)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Interactable", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float InteractableCapsuleRadius = 350.f;
	
public:	
	// Sets default values for this actor's properties
	AInteractableItem();

	// Runs when the player presses Interact.
	void OnPlayerInteract();

protected:

	// Custom function - Using UWorldObject settings for this class?
	virtual bool UseWorldObjectAssetSettings() override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Not really used, but for derived objects to be able to use it.
	virtual void Tick(float DeltaTime) override;

	// Run the right variables in here to be able to change stuff directly in Editor and see the dynamic changes
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// Runs when OnBeginOverlap() detects ACharacter
	virtual void PlayerEntersInteractable() override;

	// You can read more about this function in WorldObject.h. Should not be UFUNCTION Here, since WorldObject does it.
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							 bool bFromSweep, const FHitResult& SweepResult) override;

	// You can read more about this function in WorldObject.h. Should not be UFUNCTION Here, since WorldObject does it.
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	
	// --- Getters ---

	// Mesh Collision Tag
	virtual FName GetMeshCollisionTag() const override { return TEXT("OverlapAllDynamic"); }
	
	// DataAssetPath for subclasses to override with their own DataAsset path
	virtual FString GetDefaultSettingAssetPath() const override
	{ return TEXT("/Game/Settings/InteractableItem_Default.InteractableItem_Default"); }

	// UStaticMesh - Get the default ObjectMesh; subclasses override and swap with own mesh.
	virtual TSoftObjectPtr<UStaticMesh> GetObjectStaticMesh() const override
	{ return SettingsAsset.IsValid() ? SettingsAsset.Get()->DefaultMesh : Super::GetObjectStaticMesh(); }

	// UMaterialInterface - Get the default Material / MaterialInstance; subclasses override and swap with own material.
	virtual TSoftObjectPtr<UMaterialInterface> GetObjectMaterial() const override
	{ return SettingsAsset.IsValid() ? SettingsAsset.Get()->DefaultMaterial : Super::GetObjectMaterial(); }

	// USoundBase - Get the default SoundBase; subclasses override and swap with own sound.
	virtual TSoftObjectPtr<USoundBase> GetObjectSound() const override
	{ return SettingsAsset.IsValid() ? SettingsAsset.Get()->DefaultSound : Super::GetObjectSound(); }

	// Subclasses override this to assign delegates to OnBeginOverlap or OnEndOverlap with .AddDynamic.
	// Since virtual functions only run the most derived version, you have to include all your colliders in every subclass.
	virtual TArray<UPrimitiveComponent*> GetAllOverlapComponents() const override;
};
