// Fill out your copyright notice in the Description page of Project Settings.


#include "BossController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Boss.h"

ABossController::ABossController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);
}

void ABossController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (InPawn == nullptr) return;

	ABoss* Boss = Cast<ABoss>(InPawn);
	if (Boss)
	{
		if (Boss->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(Boss->GetBehaviorTree()->BlackboardAsset));
		}
	}
}
