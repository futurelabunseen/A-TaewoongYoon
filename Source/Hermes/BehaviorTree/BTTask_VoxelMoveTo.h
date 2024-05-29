// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CPathVolume.h"
#include "BTTask_VoxelMoveTo.generated.h"

/**
 * 
 */


struct FBT_VoxelMoveTo
{
	FVector GoalLocation;
	FCPathResult PathResult;
	uint16 CurrentIndex;
};

UCLASS()
class HERMES_API UBTTask_VoxelMoveTo : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_VoxelMoveTo();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;
	


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxel Path")
	FBlackboardKeySelector GoalLocationKey;//목적지 블랙보드 키

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
	void MoveAlongPath(APawn* Pawn , const TArray<FCPathNode>& PathPoints , uint16& CurrentIndex , float DeltaSeconds);
};
