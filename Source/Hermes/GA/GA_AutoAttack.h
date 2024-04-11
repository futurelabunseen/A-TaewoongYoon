// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AutoAttack.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class HERMES_API UGA_AutoAttack : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_AutoAttack();



private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> AutoAttackIcon;
};
