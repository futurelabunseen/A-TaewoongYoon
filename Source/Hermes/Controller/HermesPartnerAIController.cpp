// Fill out your copyright notice in the Description page of Project Settings.


#include "HermesPartnerAIController.h"
#include "Character\HermesPlayerCharacter.h"
#include "BehaviorTree\BlackboardData.h"
#include "BehaviorTree\BlackboardComponent.h"
#include "HermesBlackBoardKeyTable.h"

AHermesPartnerAIController::AHermesPartnerAIController()
{
}

void AHermesPartnerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AHermesPlayerCharacter* currentChar = Cast<AHermesPlayerCharacter>(GetCharacter());
	if (currentChar)
	{
		Blackboard->SetValueAsVector(BBKEY_LEADERPOSITION, currentChar->GetLeader().GetActorLocation());
	}
}
