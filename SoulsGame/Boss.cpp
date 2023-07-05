// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "AttributeComponent.h"
#include "HealthBarComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "BossController.h"
#include "SoulsMainCharacter.h"

ABoss::ABoss() :
	Attack1(TEXT("Attack1")),
	Attack2(TEXT("Attack2")),
	BaseDamage(20.f)

	// Sets default values

{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());

	LeftHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Hand Box"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("LeftHandBone"));
	RightHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Hand Box"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("RightHandBone"));

	//EnemyWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("EnemyWeaponCollision"));
	//EnemyWeaponCollision->SetupAttachment(GetMesh(), FName("SwordSocket"));
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::AgroSphereOverlap);
	AttackRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::AttackRangeOverlap);
	AttackRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::AttackRangeEndOverlap);
	LeftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoss::OnLeftHandOverlap);
	RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoss::OnRightHandOverlap);

	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	
	//EnemyWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	//EnemyWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	BossController = Cast<ABossController>(GetController());


	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint);

	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint2);

	if (BossController)
	{
		BossController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		BossController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);

		BossController->RunBehaviorTree(BehaviorTree);
	}

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &ABoss::PawnSeen);
	}
}

void ABoss::HandleDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactAnimationMontage)
	{
		AnimInstance->Montage_Play(DeathAnimationMontage);

		const int32 SelectRandomAnimation = FMath::RandRange(0, 1);
		FName DeathName = FName();

		switch (SelectRandomAnimation)
		{
		case 0:
			DeathName = FName("Death1");
			DeathState = EDeathState::EDS_Death1;
			break;

		case 1:
			DeathName = FName("Death2");
			DeathState = EDeathState::EDS_Death2;
			break;
		

		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(DeathName, DeathAnimationMontage);
	}

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
}

void ABoss::HandleAttackMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MeleeAttackAnimationMontage)
	{
		AnimInstance->Montage_Play(MeleeAttackAnimationMontage);
		AnimInstance->Montage_JumpToSection(Section, MeleeAttackAnimationMontage);
	}
}

FName ABoss::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section{ FMath::RandRange(0, 1) };
	switch (Section)
	{
	case 1:
		SectionName = Attack1;
		break;
	case 2:
		SectionName = Attack2;
		break;
	}
	return SectionName;
}

void ABoss::PawnSeen(APawn* SeenPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("Character detected!"));
}

void ABoss::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	auto SoulCharacter = Cast<ASoulsMainCharacter>(OtherActor);

	if (SoulCharacter)
	{
		//Set value of the Blackboard key
		BossController->GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerTarget"), SoulCharacter);
	}
}

void ABoss::AttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	auto SoulCharacter = Cast<ASoulsMainCharacter>(OtherActor);
	if (SoulCharacter)
	{
		IsInAttackRange = true;
		if (BossController)
		{
			BossController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}

void ABoss::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;
	auto SoulCharacter = Cast<ASoulsMainCharacter>(OtherActor);
	if (SoulCharacter)
	{
		IsInAttackRange = false;
		if (BossController)
		{
			BossController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void ABoss::OnLeftHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DealDamage(OtherActor);
}

void ABoss::OnRightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DealDamage(OtherActor);
}

void ABoss::ActivateLeftHand()
{
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABoss::DeactivateLeftHand()
{
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABoss::ActivateRightHand()
{
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABoss::DeactivateRightHand()
{
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABoss::DealDamage(AActor* Victim)
{
	if (Victim == nullptr) return;
	auto Character = Cast<ASoulsMainCharacter>(Victim);
	if (Character)
	{
		UGameplayStatics::ApplyDamage(
			Character,
			BaseDamage,
			BossController,
			this,
			UDamageType::StaticClass()
		);
	}
}



void ABoss::HandleHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactAnimationMontage)
	{
		AnimInstance->Montage_Play(HitReactAnimationMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactAnimationMontage);
	}
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget)
	{
		const double DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();
		if (DistanceToTarget > CombatRadius)
		{
			CombatTarget = nullptr;

			if (HealthBarWidget)
			{
				HealthBarWidget->SetVisibility(false);
			}
		}
	}
}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABoss::GetHit(const FVector& ImpactPoint)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}

	if (Attributes && Attributes->IsAlive())
	{
		DirectionalHit(ImpactPoint);
	}
	else
	{
		HandleDeathMontage();
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

void ABoss::DirectionalHit(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower impact point to the Enemy's Actor Location Z to make sure that both vectors are parallel to the ground
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector EnemyToHit = (ImpactPoint - GetActorLocation()).GetSafeNormal(); // normalized 

	// Calculating the Dot product to find the angle between these two vectors

	// Forward * EnemyToHit = |Forward| * |EnemyToHit| * cos(theta)
	// |Forward| = 1, |EnemyToHit| = 1, we end up with Forward * EnemyToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, EnemyToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta (the angle)
	double Theta = FMath::Acos(CosTheta); // we get the angle in radians
	// We need to convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// Calculating the Cross product

	const FVector CrossProduct = FVector::CrossProduct(Forward, EnemyToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	HandleHitReactMontage(FName(Section));
}

float ABoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	CombatTarget = EventInstigator->GetPawn();
	return DamageAmount;
}

void ABoss::HandleDamage(float DamageAmount)
{
	if (Attributes && HealthBarWidget)
	{
		Attributes->SetDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}



