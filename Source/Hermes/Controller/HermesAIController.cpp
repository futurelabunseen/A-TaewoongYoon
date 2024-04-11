// Fill out your copyright notice in the Description page of Project Settings.


#include "HermesAIController.h"
#include "BehaviorTree\BehaviorTree.h"
#include "BehaviorTree\BlackboardData.h"
#include "BehaviorTree\BlackboardComponent.h"
#include "HermesBlackBoardKeyTable.h"

AHermesAIController::AHermesAIController()
{
}

void AHermesAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BlackBoardAsset, BlackboardPtr))
	{
		bool RunResult = RunBehaviorTree(BehaviorTreeAsset);
		ensure(RunResult);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Run AI"));
	}
}

void AHermesAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AHermesAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunAI();
	
}

void AHermesAIController::OnUnPossess()
{
	Super::OnUnPossess();
	StopAI();
	
}

void AHermesAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
