// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulsMainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Item.h"
#include "SoulWeapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "SoulCharacterHUD.h"
#include "SoulCharacterWidget.h"
#include "AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyController.h"
#include "BossController.h"
#include "BehaviorTree/BlackboardComponent.h"



ASoulsMainCharacter::ASoulsMainCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)
	//Health(100.f),
	//MaxHealth(100.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}


// Called to bind functionality to input
void ASoulsMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASoulsMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASoulsMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASoulsMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASoulsMainCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ASoulsMainCharacter::EKey);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ASoulsMainCharacter::Attack);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ASoulsMainCharacter::Dodge);
}


// Called when the game starts or when spawned
void ASoulsMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeSoulCharacterWidget();

	UAttributeComponent* att = NewObject<UAttributeComponent>();
	att->CurrentHealth = 100.f;
	att->MaxHealth = 100.f;
	att->CurrentStamina = 120.f;
	att->MaxStamina = 120.f;
	Attributes = att;	

}

void ASoulsMainCharacter::SetSwordCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetSwordBox())
	{
		EquippedWeapon->GetSwordBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

float ASoulsMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes)
	{
		if (Attributes->CurrentHealth <= 0)
		{
			HandleDeathMontage();

			auto EnemyController = Cast<AEnemyController>(EventInstigator);
			if (EnemyController)
			{
				EnemyController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("SoulCharacterDead")), true);
			}
		}

		else if (Attributes->CurrentHealth <= 0)
		{
			HandleDeathMontage();

			auto BossController = Cast<ABossController>(EventInstigator);
			if (BossController)
			{
				BossController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("SoulCharacterDead")), true);
			}
		}

		Attributes->SetDamage(DamageAmount);	
	}



	

	SetHUDHealth();

	return DamageAmount;
}



void ASoulsMainCharacter::InitializeSoulCharacterWidget()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASoulCharacterHUD* SoulCharacterHUD = Cast<ASoulCharacterHUD>(PlayerController->GetHUD());
		if (SoulCharacterHUD)
		{
			SoulCharacterWidget = SoulCharacterHUD->GetSoulCharacterWidget();
			if (SoulCharacterWidget && Attributes)
			{
				SoulCharacterWidget->SetHealthBarPercent(Attributes->GetHealthPercent());
				SoulCharacterWidget->SetStaminaBarPercent(100.f);
			}
		}
	}
}

void ASoulsMainCharacter::SetHUDHealth()
{
	if (SoulCharacterWidget && Attributes)
	{
		SoulCharacterWidget->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ASoulsMainCharacter::HandleDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathAnimationMontage)
	{
		AnimInstance->Montage_Play(DeathAnimationMontage);
	}
}

void ASoulsMainCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}
}



void ASoulsMainCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void ASoulsMainCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void ASoulsMainCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
}

void ASoulsMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
}

void ASoulsMainCharacter::EKey()
{
	ASoulWeapon* OverlapWithWeapon = Cast<ASoulWeapon>(OverlapWithItem);
	if (OverlapWithWeapon)
	{
		OverlapWithWeapon->EquipItem(GetMesh(), FName("SwordSocket"), this, this);
		SoulsCharacterState = ESoulsCharacterState::ESCS_EquippedWeapon;
		OverlapWithItem = nullptr;
		EquippedWeapon = OverlapWithWeapon;
	}
}

void ASoulsMainCharacter::Attack()
{
	const bool CanAttack = 
		ActionState == EActionState::EAS_Unoccupied && 
		SoulsCharacterState != ESoulsCharacterState::ESCS_Unequipped;

	if (CanAttack)
	{
		HandleAttackMontage();
		ActionState = EActionState::EAS_AttackingState;
	}
	
}

void ASoulsMainCharacter::Dodge()
{
	if (!HasEnoughStamina())
	{
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeAnimationMontage)
	{
		AnimInstance->Montage_Play(DodgeAnimationMontage);
	}

	if (Attributes && SoulCharacterWidget)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SoulCharacterWidget->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

bool ASoulsMainCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetCurrentStamina() > Attributes->GetDodgeCost();
}

void ASoulsMainCharacter::HandleAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackAnimationMontage)
	{
		AnimInstance->Montage_Play(AttackAnimationMontage);
		const int32 SelectRandomAnimation = FMath::RandRange(0, 1);
		FName AttackName = FName();

		switch (SelectRandomAnimation)
		{
		case 0:
			AttackName = FName("Attack1");
			break;

		case 1:
			AttackName = FName("Attack2");
			break;

		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(AttackName, AttackAnimationMontage);
	}
}

void ASoulsMainCharacter::AttackFinished()
{
	ActionState = EActionState::EAS_Unoccupied;
}


// Called every frame
void ASoulsMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && SoulCharacterWidget)
	{
		Attributes->RegenerateStamina(DeltaTime);
		SoulCharacterWidget->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}