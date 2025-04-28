// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindPlayerLocation.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get player character
	if (auto* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		//get player location to use as origin
		FVector const PlayerLocation = player->GetActorLocation();
		if (SearchRandom)
		{
			FNavLocation loc;

			//generate random location near player
			if (auto* const NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld()))
			{
				//try to get random point near player
				if (NavSys->GetRandomPointInNavigableRadius(PlayerLocation, SearchRadius, loc))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), loc.Location);
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
			}
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerLocation);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
