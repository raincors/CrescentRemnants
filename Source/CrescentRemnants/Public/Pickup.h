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
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Component", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	TObjectPtr<UStaticMeshComponent> ObjectMeshComp;
	
	// StaticMeshComponent - StaticMesh
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Component", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	TSoftObjectPtr<UStaticMesh> ObjectMesh;

	// StaticMeshComponent - Material / MaterialInstance (via UMaterialInterface)
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Component", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	TSoftObjectPtr<UMaterialInterface> ObjectMaterial;

	// CapsuleComponent (for overlapping with ACharacter)
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Component", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	TObjectPtr<UCapsuleComponent> ObjectCapsuleComp;

	// PointLightComponent
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Component", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	TObjectPtr<UPointLightComponent> ObjectPointLightComp;

	// AudioComponent
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Component", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	TObjectPtr<UAudioComponent> ObjectAudioComp;

	// AudioComponent - USoundBase (sound to play)
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Override|Component", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsInteractable = false;

	// Bool - Destroy actor when picked up? - Default = true for APickup, but can be overridden by subclasses.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Settings", meta = (AllowPrivateAccess = "true"))
	bool bDestroyOnPickup = false;

	// Does the object float?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Settings", meta = (AllowPrivateAccess = "true"))
	bool bEnableFloating = true;
	
	// ObjectMesh - Rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Mesh", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FRotator ObjectMeshRotation = FRotator(0.f, 0.f, 0.f);
	
	// ObjectMesh - Scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Mesh", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FVector ObjectMeshScale = FVector(0.5f, 0.5f, 0.5f);

	// CapsuleComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FVector CapsuleLocation = FVector(0.f, 0.f, 0.f);

	// CapsuleComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FRotator CapsuleRotation = FRotator(0.f, 0.f, 0.f);

	// CapsuleComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FVector CapsuleScale = FVector(0.5f, 0.5f, 0.5f);

	// CapsuleComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"), meta=(Tooltip="The radius of the capsule. Should be equal or less than half-height."), meta = (EditCondition = "bAllowSettingsOverride"))
	float CapsuleRadius = 80.f;

	// CapsuleComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"), meta=(Tooltip="The half-height of the capsule. Should be equal or greater than the radius."), meta = (EditCondition = "bAllowSettingsOverride"))
	float CapsuleHalfHeight = 80.f;

	// PointLightComponent Location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FVector ObjectLightLocation = FVector(0.f, 0.f, -50.f);

	// PointLightComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	FColor LightColour = FColor::White;

	// PointLightComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float LightIntensity = 400.f;

	// PointLightComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float LightAttenuationRadius = 100.f;

	// PointLightComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	float LightSourceRadius = 100.f;

	// PointLightComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bAllowSettingsOverride"))
	bool bIsLightOn = true;

	// FloatingHeight default at 0. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Visuals", meta = (EditCondition = "bEnableFloating", EditConditionHides))
	float FloatingDistance = 0.f;

	// FloatingSpeed default at 0.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Visuals", meta = (EditCondition = "bEnableFloating", EditConditionHides))
	float FloatingSpeed = 0.f;

	// Debug - Is the capsule collider going to be visible in-game?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Debug", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bDebugEnabled"))
	bool bDebugCapsuleVisibility = false;
	
public:
	// Constructors should be public in Unreal Engine. Otherwise, you will have troubles spawning your object.
	APickup();

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
