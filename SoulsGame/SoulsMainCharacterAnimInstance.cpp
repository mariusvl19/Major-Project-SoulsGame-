// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulsMainCharacterAnimInstance.h"
#include "SoulsMainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USoulsMainCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SoulsCharacter = Cast<ASoulsMainCharacter>(TryGetPawnOwner());
	if (SoulsCharacter)
	{
		SoulsCharacterMovement = SoulsCharacter->GetCharacterMovement();
	}
}

void USoulsMainCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SoulsCharacterMovement)
	{
		Speed = UKismetMathLibrary::VSizeXY(SoulsCharacterMovement->Velocity);
		isFalling = SoulsCharacterMovement->IsFalling();
		SoulsCharacterState = SoulsCharacter->GetSoulsCharacterState();
	}
}
