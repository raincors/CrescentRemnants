// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableItem.h"

#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

/** Worth knowing is that any class deriving off of AWorldObject, runs its constructor to initialize components and apply
 * a default set of variables. The C++ implementation of doing things; the back-up plan, or Plan B.
 *
 * All components initialized here in the constructor, will be inherited and already initialized for subclasses.
 * Inherited classes should only add new components or modify the existing components.
 */
AInteractableItem::AInteractableItem()
{
	// Set this actor to call Tick() every frame. Set to false as default in WorldObject.
	PrimaryActorTick.bCanEverTick = false;
	
	bOneTimeUseOnly = false;

	// Changing the inherited CapsuleCompSize to be the default Interactable settings
	ObjectCapsuleComp->SetCapsuleSize(InteractableCapsuleRadius, InteractableCapsuleHalfHeight);
	
	// Setting a constructor default DebugColour and LightColour
	DebugColour = FColor::Green;
	LightColour = FColor::Emerald;
	
	// InteractableOverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractableOverlapSphere"));

	if (InteractableOverlapSphere)
	{
		InteractableOverlapSphere->SetupAttachment(ObjectRoot);
		InteractableOverlapSphere->InitSphereRadius(InteractableOverlapSphereRadius);
	}
}

bool AInteractableItem::UseWorldObjectAssetSettings()
{
	if (Super::UseWorldObjectAssetSettings())
	{
		Super::UseWorldObjectAssetSettings();
	}
	else
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

	// TODO: Add Interactable World Settings that needs to be stored locally or something.

	
	return true;
}

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();

	// We want to enable InteractableItems to be able to collide with the player. Doing it on BeginPlay for now.
	ObjectMeshComp->SetCollisionProfileName(GetMeshCollisionTag());
}

void AInteractableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITOR

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
	if (bIsActivated && bOneTimeUseOnly) return;
	
	Super::PlayerEntersInteractable();

	ObjectPointLightComp->SetVisibility(bIsLightOn);
	
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
	if (OtherActor && Cast<ACharacter>(OtherActor))
	{
		if (bDebugEnabled)
		{
			FString objectName = this->GetName();
			check(GEngine != nullptr);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, DebugColour,TEXT("Player leaves me...! Remember me as " + objectName));
		}
		
		// TODO: Play a simple animation everytime the player enters a checkpoint, just a little wiggle or something.
	}
}

// Have to manually add in components and make sure they are cast safely
TArray<UPrimitiveComponent*> AInteractableItem::GetAllOverlapComponents() const
{
	TArray<UPrimitiveComponent*> OverlapComponents;
	OverlapComponents.Add(ObjectCapsuleComp.Get());
	// OverlapComponents.Add(Cast<UPrimitiveComponent>(InteractableOverlapSphere.Get()));
	
	return OverlapComponents;
}

void AInteractableItem::OnPlayerInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("%s was interacted with!"), *GetName());

	// Play sound if assigned
	/*if (InteractionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, GetActorLocation());
	}*/

	// Toggle the light, if enabled on the object
	if (bInteractionTogglesLight)
	{
		bIsLightOn = !bIsLightOn;
		ObjectPointLightComp->SetVisibility(bIsLightOn);
	}

	// Destroy object if set to do so
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
	

