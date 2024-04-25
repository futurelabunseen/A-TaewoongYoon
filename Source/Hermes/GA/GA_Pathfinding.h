// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Pathfinding.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class HERMES_API UGA_Pathfinding : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Pathfinding();

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility
	) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	UFUNCTION(BlueprintCallable)
	const TArray<FVector>& FindNavMeshPathSync();

	UFUNCTION(BlueprintCallable)
	void UpdatePath(const TArray<FVector>& PathPoints);

	UFUNCTION(BlueprintCallable)
	void SetGoal(FVector Goal);


	UFUNCTION(BlueprintCallable)
	TArray<FVector> ConvertCPathNode(const TArray<FCPathNode>& CPathNodes);

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ACPathVolumeHermes> VoxelVolume;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> SplineStaticMesh;

	

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	FVector GoalLocation;

	UPROPERTY(EditAnywhere)
	int32 MaxNumOfSplinePoints;

	UPROPERTY(EditAnywhere)
	float SplineTickness;

	UPROPERTY(EditAnywhere)
	float RouteZHeight;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> AirRouteMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> GroundRouteMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> WallRouteMaterial;
	
	
};
