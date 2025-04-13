// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WorldObject.h"
#include "Components/CapsuleComponent.h"
#include "Pickup.generated.h"

class UWorldObjectSettings;
class UStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;
class UPointLightComponent;
class UAudioComponent;
class USoundBase;

/**
 * APickup, collectible to be picked up by ACharacter. It destroys them at the end of OnBeginOverlap().
 *
 * 2 Classes inherit from this - PlayerCheckpoint and InteractableItem, passing shared components.
 *
 * Pickup Components:
 * - ObjectMeshComp (UStaticMeshComponent)  - Visible 3D model and material
 *		- ObjectMesh (UStaticMesh)
 *		- ObjectMaterial (UMaterialInterface)
 * - CapsuleComp (UCapsuleComponent)					- Collision volume
 * - PointLightComp (UPointLight)						- Optional visual glow
 * - ObjectAudioComp (UAudioComponent)					- Optional pickup sound
 */
UCLASS(Blueprintable)
class CRESCENTREMNANTS_API APickup : public AWorldObject
{
	GENERATED_BODY()

	// Unique Pickup Function for Pickup class only (called in OnBeginOverlap())
	UFUNCTION(BlueprintCallable, Category = "Pickup", meta = (AllowPrivateAccess="true"))
	void PickupItem();

protected:
	
	// --- Components: --- Note that they are named more generic in regard to usage in inherited classes.
	
	// StaticMeshComponent
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ObjectMeshComp;
	
	// StaticMeshComponent - StaticMesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UStaticMesh> ObjectMesh;

	// StaticMeshComponent - Material / MaterialInstance (via UMaterialInterface)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMaterialInterface> ObjectMaterial;

	// CapsuleComponent (for overlapping with ACharacter)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> ObjectCapsuleComp;

	// PointLightComponent
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPointLightComponent> ObjectPointLightComp;

	// AudioComponent
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAudioComponent> ObjectAudioComp;

	// AudioComponent - USoundBase (sound to play)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<USoundBase> ObjectSound;

	/** --- Properties / Variables ---
	 * Categories (in order):
	 * - Bools
	 * - ObjectMesh
	 * - CapsuleComponent
	 * - PointLightComponent
	 * - Float Settings
	 * - Debug Settings
	 */
	

	// Bool - bIsInteractable? Default = false for APickup, but can be overridden by subclasses.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Settings", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	bool bIsInteractable = false;

	// Does the object float?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Settings", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	bool bEnableFloating = true;
	
	// ObjectMesh - Rotation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Mesh", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FRotator ObjectMeshRotation = FRotator(0.f, 0.f, 0.f);
	
	// ObjectMesh - Scale
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Mesh", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FVector ObjectMeshScale = FVector(0.5f, 0.5f, 0.5f);

	// CapsuleComponent
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FVector CapsuleLocation = FVector(0.f, 0.f, 0.f);

	// CapsuleComponent
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float CapsuleRadius = 75.f;

	// CapsuleComponent
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float CapsuleHalfHeight = 75.f;

	// PointLightComponent Location
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FVector ObjectLightLocation = FVector(0.f, 0.f, -30.f);

	// PointLightComponent
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FColor LightColour = FColor::White;

	// PointLightComponent
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float LightIntensity = 400.f;

	// PointLightComponent
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float LightAttenuationRadius = 300.f;

	// PointLightComponent
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float LightSourceRadius = 100.f;

	// PointLightComponent
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	bool bIsLightOn = true;

	// FloatingHeight default at 0. 
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Float", meta = (EditCondition = "bAllowSettingsOverride"))
	float FloatingDistance = 0.f;

	// FloatingSpeed default at 0.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Float", meta = (EditCondition = "bAllowSettingsOverride"))
	float FloatingSpeed = 0.f;

	// Debug - Is the capsule collider going to be visible in-game?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Debug", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bDebugEnabled"))
	bool bDebugCapsuleVisibility = false;
	
public:
	// Constructors should be public in Unreal Engine. Otherwise, you will have troubles spawning your object.
	APickup();

	// Bool - bIsPickup? Default = true for APickup, but can be overridden by subclasses.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Settings", meta = (EditCondition = "bAllowSettingsOverride"))
	bool bIsPickup = false;

protected:
	
	// Custom function - Using UWorldObject settings for this class?
	virtual bool UseWorldObjectAssetSettings() override;

	// Runs when you start Play Mode
	virtual void BeginPlay() override;

	// Custom function for floating
	virtual void Float();

	// Runs 60 times a second
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	// Only runs in Editor, used to update objects when you update variables
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// For inherited classes to override.
	virtual void PlayerEntersInteractable();

	// Custom function with bound delegate - You can read more about this function in WorldObject.cpp.
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							 bool bFromSweep, const FHitResult& SweepResult) override;
	
	/** --- Getters ---
	 * 
	 * Virtual getters for classes to override, and replace the returns with their own instanced variables, components etc.
	 */
	
	// UStaticMesh - If you have a different StaticMesh in AssetSettings
	virtual TSoftObjectPtr<UStaticMesh> GetObjectStaticMesh() const { return ObjectMesh; }

	// UMaterialInterface - If you have a different Material in AssetSettings
	virtual TSoftObjectPtr<UMaterialInterface> GetObjectMaterial() const { return ObjectMaterial; }

	// USoundBase - If you have a different USoundBase in AssetSettings
	virtual TSoftObjectPtr<USoundBase> GetObjectSound() const { return ObjectSound; }

	// Classes need to override this with their own unique collision components, in order make them detect any overlaps.
	// They're going to have delegates bound to them in WorldObject.
	virtual TArray<UPrimitiveComponent*> GetAllOverlapComponents() const override { return {ObjectCapsuleComp.Get()}; }
};
