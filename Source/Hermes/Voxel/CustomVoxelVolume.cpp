// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/CustomVoxelVolume.h"

ACustomVoxelVolume::ACustomVoxelVolume()
{
}

void ACustomVoxelVolume::CalcFitness(CPathAStarNode& Node , FVector TargetLocation , int32 UserData)
{
	Super::CalcFitness(Node , TargetLocation , UserData);
}

bool ACustomVoxelVolume::RecheckOctreeAtDepth(CPathOctree* OctreeRef , FVector TreeLocation , uint32 Depth)
{
	bool IsFree = Super::RecheckOctreeAtDepth(OctreeRef, TreeLocation, Depth);
	if (IsFree)
	{
		bool IsWall1 = GetWorld()->LineTraceTestByChannel(TreeLocation, FVector(TreeLocation.X, TreeLocation.Y- VoxelSize*1.49, TreeLocation.Z), TraceChannel);
		bool IsWall2 = GetWorld()->LineTraceTestByChannel(TreeLocation, FVector(TreeLocation.X, TreeLocation.Y+ VoxelSize*1.49, TreeLocation.Z), TraceChannel);
		bool IsWall3 = GetWorld()->LineTraceTestByChannel(TreeLocation, FVector(TreeLocation.X- VoxelSize*1.49, TreeLocation.Y, TreeLocation.Z), TraceChannel);
		bool IsWall4 = GetWorld()->LineTraceTestByChannel(TreeLocation, FVector(TreeLocation.X+ VoxelSize*1.49, TreeLocation.Y, TreeLocation.Z), TraceChannel);
		OctreeRef->SetIsWall(IsWall1 || IsWall2 || IsWall3 || IsWall4);
	}
	return IsFree;
}


