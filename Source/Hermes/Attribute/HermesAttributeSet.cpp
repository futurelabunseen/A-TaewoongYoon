// Fill out your copyright notice in the Description page of Project Settings.


#include "HermesAttributeSet.h"

UHermesAttributeSet::UHermesAttributeSet()
	:
	HP(150.0f),
	MaxHP(200.0f)
{
}

void UHermesAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute , float& NewValue)
{
	if ( Attribute == GetHPAttribute() )
	{
		NewValue = FMath::Clamp(NewValue , 0.f , GetMaxHP());
	}
}

void UHermesAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute , float OldValue , float NewValue)
{
	UE_LOG(LogTemp , Warning , TEXT("%s (%f) -> (%f)") , *Attribute.AttributeName , OldValue , NewValue);
}
