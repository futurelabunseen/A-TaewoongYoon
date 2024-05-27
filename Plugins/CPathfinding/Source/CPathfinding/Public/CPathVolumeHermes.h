// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPathVolume.h"
#include "CPathVolumeHermes.generated.h"

/**
 * 
 */
UCLASS()
class CPATHFINDING_API ACPathVolumeHermes : public ACPathVolume
{
	GENERATED_BODY()
public:

	int GetVoxelType(const FVector& WorldLocation);

	virtual void BeginPlay() override;

	// ------- EXTENDABLE ------

// Overwrite this function to change the priority of nodes as they are selected for the path.
// Note that this is potentially called thousands of times per FindPath call, so it shouldnt be too complex (unless your graph not very dense)
	virtual void CalcFitness(CPathAStarNode& Node , FVector TargetLocation , int32 UserData) override;

	// Overwrite this function to change the default conditions of a tree being free/ocupied.
	// You may also save other information in the Data field of an Octree, as only the least significant bit is used.
	// This is called during graph generation, for every subtree including leafs, so potentially millions of times. 
	virtual bool RecheckOctreeAtDepth(CPathOctree* OctreeRef , FVector TreeLocation , uint32 Depth) override;

	inline bool ExtractIsGroundFromData(uint32 TreeUserData)
	{
		return TreeUserData & 0x00000004;
	}
	inline bool ExtractIsWallFromData(uint32 TreeUserData)
	{
		return TreeUserData & 0x00000002;
	}
private:
	void SpawnMinimapVoxel();

	//(Location,Rotation)에 StaticMesh스폰
	void SpawnMinimapStaticMesh(FVector Location,FRotator Rotation);

private:
	UPROPERTY(EditAnywhere,Category = "Minimap")
	TObjectPtr<UStaticMesh> MinimapVoxelMesh;
};