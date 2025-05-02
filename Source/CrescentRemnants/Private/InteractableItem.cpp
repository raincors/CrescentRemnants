// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableItem.h"

#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

/** Worth knowing is that any class deriving off of AWorldObject runs its constructor to initialise components and apply
 * a default set of variables. The C++ implementation of doing things; the back-up plan, or Plan B.
 *
 * All components initialised here in the constructor will be inherited and already initialised for subclasses.
 * Inherited classes should only add new components or modify the existing components.
 */
AInteractableItem::AInteractableItem()
{
	// Set this actor to call Tick() every frame. Set to false as default in WorldObject.
	PrimaryActorTick.bCanEverTick = false;
	
	// Override parent class defaults to change behaviour
	bIsPickup = false;       // This isn't a regular pickup
	bIsInteractable = true;  // Player interacts with checkpoints
	bOneTimeUseOnly = false;	 // Checkpoints are only used / activated once.

	// Changing the inherited CapsuleCompSize to be the default Interactable settings
	ObjectCapsuleComp->SetCapsuleSize(InteractableCapsuleRadius, InteractableCapsuleHalfHeight);
	
	// Setting a constructor default DebugColour and LightColour
	DebugColour = FColor::Green;
	LightColour = FColor::Emerald;
	
	InteractableOverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractableOverlapSphere"));

	if (InteractableOverlapSphere)
	{
		InteractableOverlapSphere->SetupAttachment(ObjectRoot);
		InteractableOverlapSphere->InitSphereRadius(InteractableOverlapSphereRadius);
	}
}

bool AInteractableItem::UseWorldObjectAssetSettings()
{
	// If we can find AssetSettings, use them. If not, use defaults from constructor instead.
	if (!Super::UseWorldObjectAssetSettings())
	{
		return false;
	}
	
	// Different flags, interaction-based.
	bIsPickup = SettingsAsset->bIsPickup;
	bIsInteractable = SettingsAsset->bIsInteractable;
	bDestroyOnInteract = SettingsAsset->bDestroyOnInteract;
	bInteractionTogglesLight = SettingsAsset->bInteractionTogglesLight;
	bIsActivated = SettingsAsset->bIsActivated;

	// Can we see the OverlapSphere in PlayMode?
	bDebugIsOverlapSphereVisible = SettingsAsset->bDebugIsSphereCollisionVisible;

	// InteractableOverlapSphere
	InteractableOverlapSphereRadius = SettingsAsset->DefaultSphereRadius;

	if (InteractableOverlapSphere)
	{
		InteractableOverlapSphere->SetSphereRadius(InteractableOverlapSphereRadius);
		InteractableOverlapSphere->ShapeColor = DebugColour;
		InteractableOverlapSphere->SetHiddenInGame(bDebugIsOverlapSphereVisible);
	}

	// TODO: Ensure and check all variables and bools are accounted for, both via assetSettings and the constructor.
	
	return true;
}

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();

	// We want to enable InteractableItems to be able to collide with the player. Doing it on BeginPlay for now.
	ObjectMeshComp->SetCollisionProfileName(GetMeshCollisionTag());
}

// PostEditChangeProperty needs to be Editor-only, otherwise you won't be able to build.
#if WITH_EDITOR
// For editing the C++ object properties inside the Editor. So you don't have to restart the Editor every time to see changes.
void AInteractableItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (bDebugEnabled)
	{
		// If Debugging is enabled, draw debug.
		if (InteractableOverlapSphere && bDebugIsOverlapSphereVisible)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), InteractableOverlapSphereRadius, 8, FColor::Green,
				false, -1, 0, 1.f);
		}
	}
	else
	{
		bDebugIsOverlapSphereVisible = false;
	}
}
#endif

void AInteractableItem::PlayerEntersInteractable()
{
	// Don't run the below code unnecessarily if bIsActivated & bOneTimeUseOnly.
	if (bIsActivated && bOneTimeUseOnly) return;
	
	Super::PlayerEntersInteractable();
	
	if (!bIsActivated)
	{
		// Just making a random colour for fun. :)
		ObjectPointLightComp->LightColor = FColor::MakeRandomColor();
		
		bIsActivated = true;
	}
	else 
	{
		bIsActivated = false;
	}
}
	
void AInteractableItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// The PlayerEntersInteractable function runs from APickup.
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AInteractableItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if it's ACharacter
	if (const ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		// Check if this character is controlled by a player (eliminate enemy AI from the logic)
		if (Cast<APlayerController>(Character->GetController()))
		{
			if (bDebugEnabled)
			{
				FString objectName = this->GetName();
				check(GEngine != nullptr);
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, DebugColour,TEXT("Player leaves me...! Remember me as " + objectName));
			}
		}
	}
}

// Have to manually add in components and make sure they are cast safely.
// Do not add a component more than once, for example, if calling Super::GetAllOverlapComponents.
TArray<UPrimitiveComponent*> AInteractableItem::GetAllOverlapComponents() const
{
	TArray<UPrimitiveComponent*> OverlapComponents;
	OverlapComponents.Add(ObjectCapsuleComp.Get());
	OverlapComponents.Add(Cast<UPrimitiveComponent>(InteractableOverlapSphere.Get()));
	
	return OverlapComponents;
}

void AInteractableItem::OnPlayerInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("%s was interacted with!"), *GetName());

	// Toggle the light, if enabled on the object
	if (bInteractionTogglesLight)
	{
		bIsLightOn = !bIsLightOn;
		ObjectPointLightComp->SetVisibility(bIsLightOn);
	}

	// Destroy this object if set to do so
	if (bDestroyOnInteract)
	{
		if (bDebugEnabled)
		{
			FString objectName = this->GetName();
			check(GEngine != nullptr);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, DebugColour, TEXT("Goodbye World! ...but remember me as " + objectName));
		}
		Destroy();
	}
}
	

