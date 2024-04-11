// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameplayTagContainer.h"
#include "TargetLockComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HERMES_API UTargetLockComponent : public USphereComponent
{
	GENERATED_BODY()

public:	
	UTargetLockComponent();

	UFUNCTION(BlueprintCallable)
	const AActor* GetTargetActor() const;

	UFUNCTION()
	void ChangeTargetActorClockwise();

	UFUNCTION()
	void ChangeTargetActorCounterClockwise();


	void InputPressed();

	UFUNCTION()
	void InputReleased();


protected:
	virtual void BeginPlay() override;
	void TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* a) override;

	UFUNCTION()
    void HandleOverlapBegin(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult
	);

	UFUNCTION()
    void HandleOverlapEnd(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex
	);
private:
	int32 GetNumOpponentCharacter() const;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACharacter> OpponentCharacterClass;

	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere)
    FGameplayTag OnTargettingTag;


	bool bIsInputPressed;
};
