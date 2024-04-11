// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Activatable.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class HERMES_API UGA_Activatable : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Activatable();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle ,
		const FGameplayAbilityActorInfo* ActorInfo ,
		const FGameplayAbilityActivationInfo ActivationInfo ,
		const FGameplayEventData* TriggerEventData
	) override;


	UFUNCTION(BlueprintCallable)
	const UMaterial* GetSkillIcon() const;

	UFUNCTION(BlueprintCallable)
	TSubclassOf<UGameplayEffect> GetCoolDownGameplayEffectClass() const { return CooldownGameplayEffectClass; }



private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> SkillIcon;
};
