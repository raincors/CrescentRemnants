// Fill out your copyright notice in the Description page of Project Settings.


#include "RemnantsProgressBar.h"
#include "Components/ProgressBar.h"

void URemnantsProgressBar::SetPercent(float Percent)
{
	if (RemnProgressBar)
	{
		RemnProgressBar->SetPercent(Percent);
	}
}

