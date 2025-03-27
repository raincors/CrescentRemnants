// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"

bool UCustomCharacterMovementComponent::DetectLedge(FVector& OutLedgePosition)
{
	if (!CharacterOwner) return false;

	FVector Start = CharacterOwner->GetActorLocation();
	FVector Forward = CharacterOwner->GetActorForwardVector();
	FVector End = Start + (Forward * ForwardVectorLedgeDetectRange);

	// Trace to find ledge
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CharacterOwner);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

	if (bHit)
	{
		// If a ledge is detected, return the position
		OutLedgePosition = HitResult.Location;
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3.0f);
		return true;
	}

	return false;
}

