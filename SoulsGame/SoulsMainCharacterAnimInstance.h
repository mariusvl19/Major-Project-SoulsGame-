// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SoulsCharacterTypes.h"
#include "SoulsMainCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API USoulsMainCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	class ASoulsMainCharacter* SoulsCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* SoulsCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isFalling;

	UPROPERTY(BlueprintReadOnly, Category = "MovementWithWeapon | SoulsCharacter")
	ESoulsCharacterState SoulsCharacterState;
};
