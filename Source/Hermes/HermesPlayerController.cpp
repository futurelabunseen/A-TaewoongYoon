// Fill out your copyright notice in the Description page of Project Settings.


#include "HermesPlayerController.h"
#include "EnhancedInputComponent.h"
#include "HermesCharacter.h"

AHermesPlayerController::AHermesPlayerController()
{}

void AHermesPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AHermesPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	enhancedInputComponent->BindAction(NextCharacterAction, ETriggerEvent::Started, this, &AHermesPlayerController::ChangeToNextChar);
	enhancedInputComponent->BindAction(PreviousCharacterAction, ETriggerEvent::Started, this, &AHermesPlayerController::ChangeToPreviousChar);
}

void AHermesPlayerController::ChangeToNextChar()
{
	AHermesCharacter* currentChar = CastChecked<AHermesCharacter>(GetCharacter());
	if (IsValid(currentChar->NextCharacter.Get()))
	{
		Possess(currentChar->NextCharacter.Get());
	}
}

void AHermesPlayerController::ChangeToPreviousChar()
{
	AHermesCharacter* currentChar = CastChecked<AHermesCharacter>(GetCharacter());
	if (IsValid(currentChar->PreviousCharacter.Get()))
	{
		Possess(currentChar->PreviousCharacter.Get());
	}
}
