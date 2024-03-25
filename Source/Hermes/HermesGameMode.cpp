// Copyright Epic Games, Inc. All Rights Reserved.

#include "HermesGameMode.h"
#include "HermesCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

AHermesGameMode::AHermesGameMode()
{
}

void AHermesGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (GetWorld())
	{
		FVector SpawnPosition = InitPosition;
		bool possessPlayer = false;

		for (const auto& initChar : InitCharacters)
		{
			APawn* SpawnedActor = CastChecked<APawn>(GetWorld()->SpawnActor(initChar));
			if (SpawnedActor)
			{
				SpawnedActor->SetActorLocation(SpawnPosition);
				SpawnPosition.X += InitInterval;
				if (!possessPlayer)
				{
					NewPlayer->Possess(SpawnedActor);
					possessPlayer = true;
				}
			}
		}
	}
}
