// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WorldObject.h"
#include "Pickup.generated.h"

class UWorldObjectSettings;
class UStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;
class UCapsuleComponent;
class UPointLightComponent;

/**
 * APickup, collectible to be picked up by ACharacter. It destroys them via Guardian BP during overlap, via bIsPickup.
 *
 * 3 Classes inherit from this - PlayerCheckpoint, InteractableItem (unused), MovingPlatform; passing shared components.
 *
 * Pickup Components:
 * - ObjectMeshComp (UStaticMeshComponent)  - Component for storing 3D Mesh and Material
 * - ObjectMesh (UStaticMesh)				- Mesh
 * - ObjectMaterial (UMaterialInterface)	- Material or MaterialInstance, through the MaterialInterface
 * - CapsuleComp (UCapsuleComponent)		- Collision volume, via a capsule
 * - PointLightComp (UPointLight)			- Component for creating visual glow / light
 */
UCLASS(Blueprintable)
class CRESCENTREMNANTS_API APickup : public AWorldObject
{
	GENERATED_BODY()

#if WITH_EDITOR
	
	// Unique Pickup Function for Pickup class only; but now just used for debugging so set to be Editor_Only.
	UFUNCTION(BlueprintCallable, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	void PickupItem();
	
#endif

protected:
	
	// --- Components: --- Note that they are named more generic in regard to usage in inherited classes.
	
	// StaticMeshComponent
	UPROPERTY(BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ObjectMeshComp;
	
	// StaticMeshComponent - StaticMesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UStaticMesh> ObjectMesh;

	// StaticMeshComponent - Material / MaterialInstance (via UMaterialInterface)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMaterialInterface> ObjectMaterial;

	// CapsuleComponent (for overlapping with ACharacter)
	UPROPERTY(BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> ObjectCapsuleComp;

	// PointLightComponent
	UPROPERTY(BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPointLightComponent> ObjectPointLightComp;

	/** --- Properties / Variables ---
	 * Categories (in order):
	 * - Debug
	 * - Object Bool
	 * - ObjectMesh
	 * - CapsuleComponent
	 * - Float Settings
	 * - PointLightComponent
	 */

	// Debug - Is the capsule collider going to be visible in-game?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Debug", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "bDebugEnabled"))
	bool bDebugCapsuleVisibility = false;

	// You can find Object Bool - "bIsPickup" below under the public accessor.
	
	// Object Bool - bIsInteractable - Set true for objects the player can interact with
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsInteractable = false;

	// Object Bool - Does the object float? Default = true for APickup, but can be overriden by subclasses.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Settings", meta = (AllowPrivateAccess = "true"))
	bool bEnableFloating = true;

	// Object Bool - If the object floats, do you want to allow individual custom float settings for instances?
	UPROPERTY(EditAnywhere, Category = "Override|Settings", meta = (AllowPrivateAccess = "true", EditCondition = "bEnableFloating",
		ToolTip = "If true, you can't override the float settings for this object instance via changing defaults in DataAssets."))
	bool bUseCustomFloatSettings = false;

	// Object Bool - Is lighting turned on for the object? Default = true.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"))
	bool bIsLightOn = true;

	// Object Bool - Does overlapping with the object turn on or off the light?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"))
	bool bOverlapTogglesLight = true;


	// ObjectMesh - Location of the mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Mesh", meta = (AllowPrivateAccess = "true"))
	FVector ObjectMeshOffset = FVector(0.f, 0.f, 0.f);
	
	// ObjectMesh - Rotation of the mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Mesh", meta = (AllowPrivateAccess = "true"))
	FRotator ObjectMeshRotation = FRotator(0.f, 0.f, 0.f);
	
	// ObjectMesh - Scale of the mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Mesh", meta = (AllowPrivateAccess = "true"))
	FVector ObjectMeshScale = FVector(0.5f, 0.5f, 0.5f);

	
	// CapsuleComponent - Where is the capsuleComponent placed locally on your object?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"))
	FVector CapsuleLocationOffset = FVector(0.f, 0.f, 0.f);

	// CapsuleComponent - How is the capsuleComponent rotated locally on your object?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"))
	FRotator CapsuleRotationOffset = FRotator(0.f, 0.f, 0.f);

	// CapsuleComponent - Capsule radius (width). Should never be higher than CapsuleHalfHeight, that causes issues / possible crashes.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"))
	float CapsuleRadius = 60.f;

	// CapsuleComponent - Half the height of the capsule. Should never be lower than CapsuleRadius, that causes issues / possible crashes.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Capsule", meta = (AllowPrivateAccess = "true"))
	float CapsuleHalfHeight = 60.f;


	// Float Settings - Which axis should the object float along? The default is Z-axis (up/down).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Float", meta = (AllowPrivateAccess = "true", EditCondition = "bEnableFloating",
		ToolTip = "The axis along which the object will float. Use (1,0,0) for X, (0,1,0) for Y, or (0,0,1) for Z"))
	FVector FloatAxis = FVector(0.f, 0.f, 1.f);

	// Float Settings - Starting offset in the float range (-1.0 to 1.0) where 0.0 is center, 1.0 is top, -1.0 is bottom.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Float", meta = (AllowPrivateAccess = "true", EditCondition = "bEnableFloating", 
		ClampMin = "-1.0", ClampMax = "1.0"))
	float FloatStartPosition = 0.0f;

	// Current Float Position between -1.0 to 1.0. 
	float CurrentFloatPosition = 0.0f;

	// Float Settings - Does the object start moving in a positive direction?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Float", meta = (AllowPrivateAccess = "true", EditCondition = "bEnableFloating",
		ToolTip = "If true, movement will start in the positive direction along the chosen axis. If false, it will start in the negative direction."))
	bool bStartInPositiveDirection = true;

	// Float Settings - Is the platform currently moving in a positive direction? Leave default at false.
	bool bIsMovingInPositiveDirection = false;

	// Float Settings - Should we use sine wave movement (smooth) or ping-pong movement (consistent)?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Float", meta = (AllowPrivateAccess = "true", EditCondition = "bEnableFloating"))
	bool bUseSineWave = true;
	
	// Float Settings - FloatingDistance default at 10.f. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Float", meta = (AllowPrivateAccess = "true", EditCondition = "bEnableFloating"))
	float FloatingDistance = 10.f;

	// Float Settings - FloatingSpeed default at 2.2f.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Float", meta = (AllowPrivateAccess = "true", EditCondition = "bEnableFloating"))
	float FloatingSpeed = 2.2f;

	// Float Settings - When about to switch directions, how long do you stop before continuing to float? Default at 0.0f.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Float", meta = (AllowPrivateAccess = "true", EditCondition = "bEnableFloating"))
	float FloatPauseDuration = 0.0f;

	// Float Settings - Handle for waiting the amount of time needed for the delay, and stored counter.
	FTimerHandle FloatPauseTimerHandle;
	float StoredFloatPausedTime = 0.0f;

	// Float Setting - Is floating paused?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override|Float", meta = (AllowPrivateAccess = "true", EditCondition = "bEnableFloating"))
	bool bFloatIsPaused = false;
	
	// PointLightComponent - Where is your lightComponent locally placed according to the object transform?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"))
	FVector ObjectLightLocation = FVector(0.f, 0.f, -40.f);

	// PointLightComponent - How strong is the light? 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"))
	float LightIntensity = 400.f;

	// PointLightComponent - How far does the outer lightSphere reach out? This quickly becomes expensive with large values.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"))
	float LightAttenuationRadius = 200.f;

	// PointLightComponent - How far does the innermost lightSphere reach?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"))
	float LightSourceRadius = 100.f;

	// PointLightComponent - What colour is your light?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Light", meta = (AllowPrivateAccess = "true"))
	FColor LightColour = FColor::Yellow;
	
public:
	// Constructors should be public in Unreal Engine. Otherwise, you will have trouble spawning your object.
	APickup();

	// Object Bool - bIsPickup - Set true ONLY for objects that can be collected and removed from the world
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Override|Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsPickup = true;

protected:

	// Using UWorldObject settings for this class and adding additional Pickup features.
	virtual bool UseWorldObjectAssetSettings() override;
	
	// For inherited classes to override - activated on PlayerOverlap with an object if an interactable / checkpoint.
	virtual void PlayerEntersInteractable();

#if WITH_EDITOR
	
	// To be used when using custom instanced float settings and wanting to retrieve settings from the assetSettings.
	UFUNCTION(CallInEditor, Category = "Override|Float", meta = (AllowPrivateAccess = "true",EditCondition = "bEnableFloating", EditConditionHides))
	virtual void RetrieveFloatAssetSettings();
	
	// Used to draw a debugLine for the float distance. Only to be run inside the Editor in this class.
	virtual void OnConstruction(const FTransform& Transform) override;

	// Only runs in Editor, used to update objects when you update variables
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;

	// Function to handle unpausing movement
	virtual void UnpauseMovement();
	
	// Custom function for floating, which subclasses can override.
	virtual void Float();

	// Runs 60 times a second
	virtual void Tick(float DeltaTime) override;

	// When this object begins to overlap with another object. Read more about this function in WorldObject.cpp or Pickup.cpp.
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							 bool bFromSweep, const FHitResult& SweepResult) override;

public:

	// Virtual getters for classes to override the returns with their own instanced variables, components, etc.
	
	// UStaticMesh - Used when you have a different StaticMesh in AssetSettings
	virtual TSoftObjectPtr<UStaticMesh> GetObjectStaticMesh() const { return ObjectMesh; }

	// UMaterialInterface - Used when you have a different Material in AssetSettings
	virtual TSoftObjectPtr<UMaterialInterface> GetObjectMaterial() const { return ObjectMaterial; }

	// Classes need to override this and add all their own unique collision components, in order make them detect any overlaps.
	// They're going to have delegates bound to them in WorldObject.
	virtual TArray<UPrimitiveComponent*> GetAllOverlapComponents() const override;
};
