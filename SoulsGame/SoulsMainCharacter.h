// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SoulsCharacterTypes.h"
#include "SoulsMainCharacter.generated.h"

class UGroomComponent;
class AItem;
class UAnimMontage;
class ASoulWeapon;


UCLASS()
class SOULSGAME_API ASoulsMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASoulsMainCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetSwordCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;


protected:

	virtual void BeginPlay() override;

	/** Functions that handle input for character */
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void EKey();
	void Attack();
	void Dodge();

	bool HasEnoughStamina();

	/** Functions that handle montages */

	void HandleAttackMontage();

	UFUNCTION(BlueprintCallable)
	void AttackFinished();

	void HandleDeathMontage();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	
private:
	
	class UAttributeComponent* Attributes;

	void InitializeSoulCharacterWidget();
	void SetHUDHealth();

	ESoulsCharacterState SoulsCharacterState = ESoulsCharacterState::ESCS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Camera that follows the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = Eyebrows)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlapWithItem;


	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ASoulWeapon* EquippedWeapon;

	UPROPERTY()
	class USoulCharacterWidget* SoulCharacterWidget;

	/** Current health of the character */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//float Health;

	/** Maximum health of the character */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//float MaxHealth;

	/** Animation Montages */

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeAnimationMontage;


public:

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE void SetOverlapWithItem(AItem* Item) { OverlapWithItem = Item; }
	FORCEINLINE ESoulsCharacterState GetSoulsCharacterState() const { return SoulsCharacterState; }
};
