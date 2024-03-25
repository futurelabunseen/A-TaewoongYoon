// Copyright Epic Games, Inc. All Rights Reserved.

#include "HermesGameMode.h"
#include "HermesCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHermesGameMode::AHermesGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
