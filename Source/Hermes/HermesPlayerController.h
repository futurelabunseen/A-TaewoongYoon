// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HermesPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HERMES_API AHermesPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AHermesPlayerController();
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void ChangeToNextChar();
	void ChangeToPreviousChar();
private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> NextCharacterAction;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> PreviousCharacterAction;
};
