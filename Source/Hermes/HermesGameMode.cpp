// Copyright Epic Games, Inc. All Rights Reserved.

#include "HermesGameMode.h"
#include "HermesCharacter.h"
#include "HermesPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

AHermesGameMode::AHermesGameMode()
{
}

void AHermesGameMode::PostLogin(APlayerController* NewPlayer)
{
	AHermesPlayerController* hermesPlayer = CastChecked<AHermesPlayerController>(NewPlayer);
	if (GetWorld())
	{
		FVector SpawnPosition = InitPosition;
		bool possessPlayer = false;
		AHermesCharacter* firstChar = nullptr;//첫번째로 생성되는 캐릭터에 대한 포인터(예외처리용)
		AHermesCharacter* previousChar = nullptr;
		for (const auto& initChar : InitCharacters)
		{
			AHermesCharacter* SpawnedChar = CastChecked<AHermesCharacter>(GetWorld()->SpawnActor(initChar));
			SpawnedChar->SetActorLocation(SpawnPosition);
			SpawnPosition.X += InitInterval;
			if (IsValid(hermesPlayer->GetCharacter()))
			{//두번째 이후 플레이어 생성로직
				previousChar->NextCharacter = SpawnedChar;
				SpawnedChar->PreviousCharacter = previousChar;
				SpawnedChar->NextCharacter = firstChar;
				firstChar->PreviousCharacter = SpawnedChar;
				previousChar = SpawnedChar;
			}
			if (!possessPlayer)
			{//첫번째 플레이어 생성 로직
				hermesPlayer->Possess(SpawnedChar);
				firstChar = SpawnedChar;
				previousChar = firstChar;
				firstChar->PreviousCharacter = nullptr;
				firstChar->NextCharacter = nullptr;
				possessPlayer = true;
			}
		}
		
	}
}
