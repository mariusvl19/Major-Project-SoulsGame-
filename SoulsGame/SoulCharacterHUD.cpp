// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulCharacterHUD.h"
#include "SoulCharacterWidget.h"

void ASoulCharacterHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController && SoulCharacterWidgetClass)
		{
			SoulCharacterWidget = CreateWidget<USoulCharacterWidget>(PlayerController, SoulCharacterWidgetClass);
			SoulCharacterWidget->AddToViewport();
		}
	}

	
}
