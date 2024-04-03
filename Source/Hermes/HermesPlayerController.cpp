// Fill out your copyright notice in the Description page of Project Settings.


#include "HermesPlayerController.h"
#include "EnhancedInputComponent.h"
#include "HermesPlayerCharacter.h"

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
	AHermesPlayerCharacter* currentChar = CastChecked<AHermesPlayerCharacter>(GetCharacter());
	if (IsValid(currentChar->NextCharacter.Get()))
	{
		if(AController* NextController = currentChar->NextCharacter.Get()->GetController())
		{
			NextController->UnPossess();//빙의중인 Controller빙의 해제
			UnPossess();
			Possess(currentChar->NextCharacter.Get());//다음 캐릭터 빙의
			currentChar->GetAIController()->Possess(currentChar);//다시 인공지능컨트롤러에 의해 빙의됨
		}
	}
}

void AHermesPlayerController::ChangeToPreviousChar()
{
	AHermesPlayerCharacter* currentChar = CastChecked<AHermesPlayerCharacter>(GetCharacter());
	if (IsValid(currentChar->PreviousCharacter.Get()))
	{
		if (AController* PreviousController = currentChar->PreviousCharacter.Get()->GetController())
		{
			PreviousController->UnPossess();//빙의중인 Controller빙의 해제
			UnPossess();
			Possess(currentChar->PreviousCharacter.Get());//이전 캐릭터 빙의
			currentChar->GetAIController()->Possess(currentChar);//다시 인공지능컨트롤러에 의해 빙의됨
		}
	}
}
