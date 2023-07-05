// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SoulCharacterHUD.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API ASoulCharacterHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	
private:

	UPROPERTY(EditAnywhere, Category = SoulCharacter)
	TSubclassOf<class USoulCharacterWidget> SoulCharacterWidgetClass;

	UPROPERTY()
	USoulCharacterWidget* SoulCharacterWidget;
public:
	FORCEINLINE USoulCharacterWidget* GetSoulCharacterWidget() const { return SoulCharacterWidget; }
};
