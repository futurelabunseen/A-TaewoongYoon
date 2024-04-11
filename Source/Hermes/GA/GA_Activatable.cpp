// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Activatable.h"
#include "AbilitySystemComponent.h"

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

bool UGA_Activatable::CanActivateGA_Activatable(UAbilitySystemComponent* ASC) const
{
	FGameplayAbilitySpec* AbilitySpec =  ASC->FindAbilitySpecFromClass(this->GetClass());
	check(AbilitySpec);
	FGameplayAbilityActorInfo* ActorInfo = ASC->AbilityActorInfo.Get();
	check(ActorInfo);
	
	return CanActivateAbility(AbilitySpec->Handle , ActorInfo);
}




const UMaterial* UGA_Activatable::GetSkillIcon() const
{
	return SkillIcon;
}

