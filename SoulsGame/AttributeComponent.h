// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSGAME_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RegenerateStamina(float DeltaTime);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Character Attributes")
	float DodgeCost = 12.f;

	UPROPERTY(EditAnywhere, Category = "Character Attributes")
	float StaminaRate = 2.f;

public:
	/* Attributes for Health*/
	UPROPERTY(EditAnywhere, Category = "Character Attributes")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Character Attributes")
	float MaxHealth;

	/* Attributes for Stamina*/
	UPROPERTY(EditAnywhere, Category = "Character Attributes")
	float CurrentStamina;

	UPROPERTY(EditAnywhere, Category = "Character Attributes")
	float MaxStamina;

	
public:

	void SetDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive();

	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }
};
