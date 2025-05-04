// Fill out your copyright notice in the Description page of Project Settings.


#include "TextBubble.h"

#include "Framework/Commands/Contexts/UIIdentifierContext.h"

FString UTextBubble::GetMemory(int Memory)
{
	FString MemoryText;
	switch (Memory)
	{
	case 1:
			MemoryText = FString::Printf(TEXT("You remember the woods. The lush evergreen forest,\n and all the life it conceals. You remember home."));
			break;
		case 2:
			MemoryText = FString::Printf(TEXT("You remember the sea. The smell of salt; the massive\n waves crashing against the cliffs. The wind howling\n furiously."));
			break;
		case 3:
			MemoryText = FString::Printf(TEXT("You remember the people. Their lives, their hopes. \nYou remember the pain as the fire in their eyes was\n extinguished."));
			break;
		default:
			break;
			
	}
	
	return MemoryText;
	
}
FOnGetContextText(GetMemory);