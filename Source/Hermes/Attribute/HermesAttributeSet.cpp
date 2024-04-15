// Fill out your copyright notice in the Description page of Project Settings.


#include "HermesAttributeSet.h"
#include "GameplayEffectExtension.h"

UHermesAttributeSet::UHermesAttributeSet()
	:
	HP(150.0f),
	MaxHP(200.0f)
{
}

void UHermesAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute , float& NewValue)
{//currnet 값 변경전
	Super::PreAttributeChange(Attribute , NewValue);
	if (Attribute == GetHPAttribute())
    {
        NewValue = FMath::Clamp(NewValue , 0.0f , GetMaxHP());
    }
}

void UHermesAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute , float& NewValue) const
{//base 값 변경전
	if (Attribute == GetHPAttribute())
    {
        NewValue = FMath::Clamp(NewValue , 0.0f , GetMaxHP());
    }
}

void UHermesAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute , float OldValue , float NewValue)
{
	Super::PostAttributeChange(Attribute , OldValue,NewValue);
	
	UE_LOG(LogTemp , Warning , TEXT("%s (%f) -> (%f)") , *Attribute.AttributeName , OldValue , NewValue);
}

void UHermesAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
}
