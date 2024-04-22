// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPathVolume.h"
#include "CustomVoxelVolume.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class HERMES_API ACustomVoxelVolume : public ACPathVolume
{
	GENERATED_BODY()

public:
	ACustomVoxelVolume();
		// ------- EXTENDABLE ------

	// Overwrite this function to change the priority of nodes as they are selected for the path.
	// Note that this is potentially called thousands of times per FindPath call, so it shouldnt be too complex (unless your graph not very dense)
	virtual void CalcFitness(CPathAStarNode& Node , FVector TargetLocation , int32 UserData) override;

	// Overwrite this function to change the default conditions of a tree being free/ocupied.
	// You may also save other information in the Data field of an Octree, as only the least significant bit is used.
	// This is called during graph generation, for every subtree including leafs, so potentially millions of times. 
	virtual bool RecheckOctreeAtDepth(CPathOctree* OctreeRef , FVector TreeLocation , uint32 Depth) override;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMesh> WallMesh;//벽으로 판단할 Mesh


};
