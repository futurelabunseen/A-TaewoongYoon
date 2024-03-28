// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HermesAIController.generated.h"

/**
 * 
 */
UCLASS()
class HERMES_API AHermesAIController : public AAIController
{
	GENERATED_BODY()
public:
	AHermesAIController();

	void RunAI();
	void StopAI();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBlackboardData> BlackBoardAsset;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;
};
