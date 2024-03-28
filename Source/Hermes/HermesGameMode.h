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

private:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AHermesCharacter>> InitCharacters;

	UPROPERTY(EditAnywhere)
	int32 InitInterval;

};



