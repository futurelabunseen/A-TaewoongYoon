// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Activatable.h"

UGA_Activatable::UGA_Activatable()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Activatable::InputPressed(
	const FGameplayAbilitySpecHandle Handle ,
	const FGameplayAbilityActorInfo* ActorInfo ,
	const FGameplayAbilityActivationInfo ActivationInfo
)
{
	Super::InputPressed(Handle , ActorInfo , ActivationInfo);
}

void UGA_Activatable::CancelAbility(
	const FGameplayAbilitySpecHandle Handle ,
	const FGameplayAbilityActorInfo* ActorInfo , 
	const FGameplayAbilityActivationInfo ActivationInfo ,
	bool bReplicateCancelAbility
)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Activatable::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle ,
	const FGameplayAbilityActorInfo* ActorInfo ,
	const FGameplayAbilityActivationInfo ActivationInfo ,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_Activatable::EndAbility(
	const FGameplayAbilitySpecHandle Handle ,
	const FGameplayAbilityActorInfo* ActorInfo ,
	const FGameplayAbilityActivationInfo ActivationInfo ,
	bool bReplicateEndAbility ,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility,bWasCancelled);
}

const UMaterial* UGA_Activatable::GetSkillIcon() const
{
	return SkillIcon;
}
