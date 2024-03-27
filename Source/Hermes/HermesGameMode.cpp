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
		AHermesCharacter* firstChar = nullptr;//ù��°�� �����Ǵ� ĳ���Ϳ� ���� ������(����ó����)
		AHermesCharacter* previousChar = nullptr;
		for (const auto& initChar : InitCharacters)
		{
			AHermesCharacter* SpawnedChar = CastChecked<AHermesCharacter>(GetWorld()->SpawnActor(initChar));
			SpawnedChar->SetActorLocation(SpawnPosition);
			SpawnPosition.X += InitInterval;
			if (IsValid(hermesPlayer->GetCharacter()))
			{//�ι�° ���� �÷��̾� ��������
				previousChar->NextCharacter = SpawnedChar;
				SpawnedChar->PreviousCharacter = previousChar;
				SpawnedChar->NextCharacter = firstChar;
				firstChar->PreviousCharacter = SpawnedChar;
				previousChar = SpawnedChar;
			}
			if (!possessPlayer)
			{//ù��° �÷��̾� ���� ����
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
