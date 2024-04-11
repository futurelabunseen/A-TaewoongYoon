// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HermesAttributeSet.generated.h"



#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class HERMES_API UHermesAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UHermesAttributeSet();

	
	ATTRIBUTE_ACCESSORS(UHermesAttributeSet,HP);
	ATTRIBUTE_ACCESSORS(UHermesAttributeSet,MaxHP);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute , float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute , float OldValue , float NewValue) override;

protected:
	UPROPERTY(BlueprintReadOnly,Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HP;

	UPROPERTY(BlueprintReadOnly,Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHP;
};
