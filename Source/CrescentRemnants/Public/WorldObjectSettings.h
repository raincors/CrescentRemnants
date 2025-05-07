// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WorldObjectSettings.generated.h"

class AWorldObject;

/**
 * This class is a template for when you create a Data Asset inside Unreal Engine (Under Miscellaneous).
 * Once created, you can assign components and their properties like default meshes, materials, colliders, etc.
 * The other classes can then retrieve this data and apply it during class construction.
 *
 * - Asset Settings
 * - Debug Settings
 * - Transform Settings
 * - Mesh Settings
 * - BoxCollision Settings (UNUSED)
 * - SphereCollision Settings (UNUSED)
 * - CapsuleCollision Settings
 * - Float Settings
 * - Light Settings
 * - Interactable Settings (UNUSED)
 * - Platform Settings
 * - Hazard Settings (UNUSED)
 */
UCLASS(BlueprintType)
class CRESCENTREMNANTS_API UWorldObjectSettings : public UDataAsset
{
	GENERATED_BODY()

public:

	/**
	* WARNING: This function may cause the editor to become unresponsive or crash if called while making
	* other changes. Always save your work before pressing this button / running this function.
	* 
	* This function applies the current settings from this DataAsset to all WorldObjects in the level
	* that reference it.
	*/
	UFUNCTION(CallInEditor, Category = "Settings")
	void ApplySettingsToAllObjects();

	
	// 📝 Asset Settings

	// Should objects run default settings using the dataAsset (true), or from their own constructors (false)?
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bApplyDefaultAssetSettings = true;

	// Is this a pickup? (Pickups only)
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsPickup = false;

	// Is this an Interactable? (InteractableItem, or Checkpoint!)
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsInteractable = false;

	// Is this a platform?
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsPlatform = false;

	// Is this a hazard? (UNUSED)
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsHazard = false;

	// Is this object only used once? Like a pickup, or a checkpoint.
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bOneTimeUse = false;

	// Is this object floating? Pickups, platforms, etc.
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bEnableFloating = false;

	// If this object is floating, do you allow instances to have custom properties?
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (EditCondition = "bEnableFloating",
		ToolTip = "If true, you can't override the float settings for this object instance via changing defaults in DataAssets."))
	bool bUseCustomFloatSettings = false;

	// Is this activated from the start? Pickup floating, Checkpoint active, or platform moving, or hazard active etc...
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsActivated = true;

	
	// 🐛 Debug Settings (Note that some debug settings are located inside other settings, due to dependencies)
	
	// Do we enable debugging on the object?
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bEnableDebug = false;
	
	UPROPERTY(EditAnywhere, Category = "Debug")
	FColor DefaultDebugTextColour = FColor::White;

	
	// ↗️ Transform Settings
	
	UPROPERTY(EditAnywhere, Category = "Transform")
	FRotator DefaultObjectRotation = FRotator(0.f, 0.f, 0.f);
	
	UPROPERTY(EditAnywhere, Category = "Transform")
	FVector DefaultObjectScale = FVector(1.f, 1.f, 1.f);

	
	// 🌟 Mesh Settings
	
	UPROPERTY(EditAnywhere, Category = "Visuals", meta = (EditCondition = "bHasMesh", EditConditionHides))
	bool bHasMesh = true;

	UPROPERTY(EditAnywhere, Category = "Visuals", meta = (EditCondition = "bHasMesh", EditConditionHides))
	TSoftObjectPtr<UStaticMesh> DefaultMesh = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Visuals", meta = (EditCondition = "bHasMesh", EditConditionHides))
	TSoftObjectPtr<UMaterialInterface> DefaultMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = "Visuals", meta = (EditCondition = "bHasMesh", EditConditionHides))
	FVector DefaultObjectMeshLocationOffset = FVector(0.f, 0.f, 0.f);
	
	UPROPERTY(EditAnywhere, Category = "Visuals", meta = (EditCondition = "bHasMesh", EditConditionHides))
	FRotator DefaultObjectMeshRotation = FRotator(0.f, 0.f, 0.f);
	
	UPROPERTY(EditAnywhere, Category = "Visuals", meta = (EditCondition = "bHasMesh", EditConditionHides))
	FVector DefaultObjectMeshScale = FVector(1.f, 1.f, 1.f);

	
	// 📦 Box Collision Settings - UNUSED
	
	UPROPERTY(EditAnywhere, Category = "Collision")
	bool bHasBoxCollision = false;

	UPROPERTY(EditAnywhere, Category = "Collision|Box", meta = (EditCondition = "bHasBoxCollision && bEnableDebug"))
	bool bDebugIsBoxCollisionVisible = false;

	UPROPERTY(EditAnywhere, Category = "Collision|Box", meta = (EditCondition = "bHasBoxCollision", EditConditionHides))
	FVector DefaultBoxLocation = FVector(0.f, 0.f, 0.f);
	
	UPROPERTY(EditAnywhere, Category = "Collision|Box", meta = (EditCondition = "bHasBoxCollision", EditConditionHides))
	FRotator DefaultBoxRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "Collision|Box", meta = (EditCondition = "bHasBoxCollision", EditConditionHides))
	FVector DefaultBoxExtents = FVector(60.f, 60.f, 60.f);

	
	// ⚽ Sphere Collision Settings - UNUSED
	
	UPROPERTY(EditAnywhere, Category = "Collision")
	bool bHasSphereCollision = false;

	UPROPERTY(EditAnywhere, Category = "Collision|Sphere", meta = (EditCondition = "bHasSphereCollision && bEnableDebug"))
	bool bDebugIsSphereCollisionVisible = false;
	
	UPROPERTY(EditAnywhere, Category = "Collision|Sphere", meta = (EditCondition = "bHasSphereCollision", EditConditionHides))
	FVector DefaultSphereLocation = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "Collision|Sphere", meta = (EditCondition = "bHasSphereCollision", EditConditionHides))
	float DefaultSphereRadius = 60.f;

	
	// 💊 Capsule Collision Settings
	
	UPROPERTY(EditAnywhere, Category = "Collision")
	bool bHasCapsuleCollision = false;

	UPROPERTY(EditAnywhere, Category = "Collision|Capsule", meta = (EditCondition = "bHasCapsuleCollision && bEnableDebug"))
	bool bDebugIsCapsuleCollisionVisible = true;
	
	UPROPERTY(EditAnywhere, Category = "Collision|Capsule", meta = (EditCondition = "bHasCapsuleCollision", EditConditionHides))
	FVector DefaultCapsuleLocation = FVector(0.f, 0.f, 0.f);
	
	UPROPERTY(EditAnywhere, Category = "Collision|Capsule", meta = (EditCondition = "bHasCapsuleCollision", EditConditionHides))
	FRotator DefaultCapsuleRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "Collision|Capsule", meta = (EditCondition = "bHasCapsuleCollision", EditConditionHides))
	float DefaultCapsuleHalfHeight = 60.f;
	
	UPROPERTY(EditAnywhere, Category = "Collision|Capsule", meta = (EditCondition = "bHasCapsuleCollision", EditConditionHides))
	float DefaultCapsuleRadius = 60.f;;

	
	// 🛟 Float Settings 

	// Which axis should the object float along? The default is Z-axis (up/down).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float", meta = (EditCondition = "bEnableFloating", EditConditionHides,
		ToolTip = "The axis along which the object will float. Use (1,0,0) for X, (0,1,0) for Y, or (0,0,1) for Z"))
	FVector DefaultFloatAxis = FVector(0.f, 0.f, 1.f);

	// Starting offset in the float range (-1.0 to 1.0), where 0.0 is center, 1.0 is top, -1.0 is bottom.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float", meta = (EditCondition = "bEnableFloating", EditConditionHides, 
		ClampMin = "-1.0", ClampMax = "1.0"))
	float DefaultFloatStartPosition = 0.0f;

	// Does the object start moving in a positive direction?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Float", meta = (EditCondition = "bEnableFloating", EditConditionHides,
		ToolTip = "If true, movement will start in the positive direction along the chosen axis. If false, it will start in the negative direction."))
	bool bStartInPositiveDirection = true;

	// Should we use sine wave movement (smooth) or ping-pong movement (consistent)?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Float", meta = (EditCondition = "bEnableFloating", EditConditionHides))
	bool bUseSineWave = true;
	
	UPROPERTY(EditAnywhere, Category = "Float", meta = (EditCondition = "bEnableFloating", EditConditionHides))
	float DefaultFloatingDistance = 10.f;

	UPROPERTY(EditAnywhere, Category = "Float", meta = (EditCondition = "bEnableFloating", EditConditionHides))
	float DefaultFloatingSpeed = 2.2f;

	// Float Settings - When about to switch directions, how long do you stop before continuing to float?
	UPROPERTY(EditAnywhere, Category = "Float", meta = (EditCondition = "bEnableFloating", EditConditionHides))
	float DefaultFloatStopDelay = 0.0f;

	
	// 💡 Light Settings (for pickups or platforms that glow)
	
	UPROPERTY(EditAnywhere, Category = "Light")
	bool bHasLightComponent = false;

	UPROPERTY(EditAnywhere, Category = "Light", meta = (EditCondition = "bHasLightComponent", EditConditionHides))
	FVector DefaultLightLocation = FVector(0.f, 0.f, -40.f);

	UPROPERTY(EditAnywhere, Category = "Light", meta = (EditCondition = "bHasLightComponent", EditConditionHides))
	bool bIsLightOn = false;
	
	UPROPERTY(EditAnywhere, Category = "Light", meta = (EditCondition = "bHasLightComponent", EditConditionHides))
	float DefaultLightIntensity = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Light", meta = (EditCondition = "bHasLightComponent", EditConditionHides))
	float DefaultAttenuationRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Light", meta = (EditCondition = "bHasLightComponent", EditConditionHides))
	float DefaultLightSourceRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Light", meta = (EditCondition = "bHasLightComponent", EditConditionHides))
	FColor DefaultLightColour = FColor::White;
	

	// 🧩 Interactable (for interactable objects and checkpoints)
	
	UPROPERTY(EditAnywhere, Category = "Interactable", meta = ( EditCondition = "bIsInteractable", EditConditionHides))
	bool bOverlapTogglesLight = true;
	
	UPROPERTY(EditAnywhere, Category = "Interactable", meta = ( EditCondition = "bIsInteractable", EditConditionHides))
	bool bDestroyOnInteract = false;


	// 🎮 Platform Settings
	
	UPROPERTY(EditAnywhere, Category = "Movement|Platform", meta = (EditCondition = "bIsPlatform", EditConditionHides))
	bool bIsBreakable = false;

	UPROPERTY(EditAnywhere, Category = "Movement|Platform", meta = (EditCondition = "bIsPlatform", EditConditionHides))
	bool bMoveOnlyWhenPlayerOn = true;

	
	// ⚠️ Hazard Settings (For obstacles) - UNUSED

	UPROPERTY(EditAnywhere, Category = "Hazard", meta = (EditCondition = "bIsHazard", EditConditionHides))
	bool isLethal = false;
	
	UPROPERTY(EditAnywhere, Category = "Hazard", meta = (EditCondition = "bIsHazard", EditConditionHides))
	FText DefaultDeathMessage = FText::FromString("Uh-oh, you got Zoinked!");
};
