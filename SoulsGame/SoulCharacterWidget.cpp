// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulCharacterWidget.h"
#include "Components/ProgressBar.h"

void USoulCharacterWidget::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void USoulCharacterWidget::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}
