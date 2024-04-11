// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HermesAIController.h"
#include "HermesPartnerAIController.generated.h"

/**
 * 
 */
UCLASS()
class HERMES_API AHermesPartnerAIController : public AHermesAIController
{
	GENERATED_BODY()
public:
	AHermesPartnerAIController();

	virtual void Tick(float DeltaTime) override;
};

