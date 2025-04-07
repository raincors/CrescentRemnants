// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCheckpoint.h"

#include "Components/PointLightComponent.h"
#include "GameFramework/Character.h"

APlayerCheckpoint::APlayerCheckpoint()
{
	// Set this actor to call Tick() every frame. Set to false as default.
	PrimaryActorTick.bCanEverTick = true;

	// Setting default bools
	bOneTimeUseOnly = true;

	// Setting DebugColor and LightColour
	DebugColour = FColor::Magenta;
	LightColour = FColor::Purple;
}

bool APlayerCheckpoint::UseWorldObjectAssetSettings()
{
	if (!Super::UseWorldObjectAssetSettings())
	{
		return false;
	}
	
	if (ObjectCapsuleComp)
	{
		ObjectCapsuleComp->ShapeColor = DebugColour;
	}
	return true;
}

void APlayerCheckpoint::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCheckpoint::Float()
{
	float Time = GetWorld()->TimeSeconds;
	float Offset = FMath::Sin(Time * FloatingSpeed) * FloatingDistance;
	
	FVector NewLocation = InitialLocation + FVector(0.f, 0.f, Offset);
	SetActorLocation(NewLocation);
}


void APlayerCheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActivated)
	{
		// TODO: Run a line from here to the GuardianCharacter, or do some other system to help track which checkpoint to use)
	}
}

#if WITH_EDITOR

void APlayerCheckpoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

void APlayerCheckpoint::PlayerEntersInteractable()
{
	if (bIsActivated && bOneTimeUseOnly) return;
	
	Super::PlayerEntersInteractable();

	if (!bIsActivated)
	{
		// Ensuring lights are on. We want this for the checkpoint.
		bIsLightOn = true;
		ObjectPointLightComp->SetVisibility(bIsLightOn);
		
		// Just making a random colour for fun. :)
		ObjectPointLightComp->LightColor = FColor::MakeRandomColor();
		
		// the object is now activated. If oneTimeUse is enabled, this stays true.
		bIsActivated = true;
	}
	else
	{
		bIsActivated = false;
	}
	
	// TODO: Add functionality for when player first overlaps with Interactable.
}

// Commenting out the names in the parameters to silence warnings (we're not using 'em).
void APlayerCheckpoint::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && Cast<ACharacter>(OtherActor))
	{
		if (bDebugEnabled)
		{
			FString objectName = this->GetName();
	
			check(GEngine != nullptr);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, DebugColour, TEXT("Player leaves checkpoint! ... remember me as " + objectName));
		}
	}
}