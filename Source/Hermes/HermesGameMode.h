// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HermesGameMode.generated.h"

UCLASS(minimalapi)
class AHermesGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHermesGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class ACharacter>> InitCharacters;


	UPROPERTY(EditAnywhere)
	FVector InitPosition;

	UPROPERTY(EditAnywhere)
	int32 InitInterval;

};



