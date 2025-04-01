// Fill out your copyright notice in the Description page of Project Settings.


#include "RemnantHUD.h"
#include "RemnantsProgressBar.h"
#include "Blueprint/UserWidget.h"

void ARemnantHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && RemnantsProgressBarClass)
		{
			RemnantsProgressBar = CreateWidget<URemnantsProgressBar>(Controller,RemnantsProgressBarClass);
			RemnantsProgressBar->AddToViewport();
		}
	}

	
}
