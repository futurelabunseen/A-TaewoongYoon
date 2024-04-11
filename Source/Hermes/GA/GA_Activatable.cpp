// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Activatable.h"

UGA_Activatable::UGA_Activatable()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Activatable::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle ,
	const FGameplayAbilityActorInfo* ActorInfo , 
	const FGameplayAbilityActivationInfo ActivationInfo ,
	const FGameplayEventData* TriggerEventData
)
{
	CommitAbility(Handle , ActorInfo , ActivationInfo);
	Super::ActivateAbility(Handle , ActorInfo , ActivationInfo , TriggerEventData);
}


const UMaterial* UGA_Activatable::GetSkillIcon() const
{
	return SkillIcon;
}

