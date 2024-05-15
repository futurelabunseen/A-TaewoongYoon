// Fill out your copyright notice in the Description page of Project Settings.


#include "CPathVolumeHermes.h"
#include "CPathFindPath.h"

int ACPathVolumeHermes::GetVoxelType(const FVector& WorldLocation)
{
	uint32 TreeID;
	FindClosestFreeLeaf(WorldLocation , TreeID);//Voxel ID서치
	CPathOctree* CurrentOctreeNode = FindTreeByID(TreeID);//Voxel 옥트리 노드 서치
	if ( !CurrentOctreeNode )
		return 2;

	if ( CurrentOctreeNode->GetIsGround() )
	{//땅
		return 0;
	}
	else if ( CurrentOctreeNode->GetIsWall() )
	{//벽
		return 1;
	}
	else
	{//공중
		return 2;
	}
}

void ACPathVolumeHermes::BeginPlay()
{
	Super::BeginPlay();
}

void ACPathVolumeHermes::CalcFitness(CPathAStarNode& Node , FVector TargetLocation , int32 UserData)
{
	check(Node.PreviousNode);

	Node.DistanceSoFar = Node.PreviousNode->DistanceSoFar + FVector::Distance(Node.PreviousNode->WorldLocation, Node.WorldLocation);//현재노드까지 거리누적
	
	
	/*if ( ExtractIsGroundFromData(Node.TreeUserData) )
		Node.DistanceSoFar += 100;*/
	
	Node.FitnessResult = Node.DistanceSoFar + 3.5f * FVector::Distance(Node.WorldLocation, TargetLocation);//FitnessResult: A*평가값, 낮을수록 우선순위
}

bool ACPathVolumeHermes::RecheckOctreeAtDepth(CPathOctree* OctreeRef , FVector TreeLocation , uint32 Depth)
{
	bool IsFree = Super::RecheckOctreeAtDepth(OctreeRef, TreeLocation, Depth);
	if (IsFree)
	{
		float TraceAmount = VoxelSize * 3.f;
		{//벽 판단
			bool IsWall1 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X , TreeLocation.Y - TraceAmount , TreeLocation.Z) , TraceChannel);
			bool IsWall2 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X , TreeLocation.Y + TraceAmount , TreeLocation.Z) , TraceChannel);
			bool IsWall3 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X - TraceAmount , TreeLocation.Y , TreeLocation.Z) , TraceChannel);
			bool IsWall4 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X + TraceAmount , TreeLocation.Y , TreeLocation.Z) , TraceChannel);
			bool IsWall = IsWall1 || IsWall2 || IsWall3 || IsWall4;
			OctreeRef->SetIsWall(IsWall);

			//if ( !IsWall )
			//{//벽은 아니지만 대각선 아래방향에 벽이 존재하는 경우 -> 벽으로 취급
			//	bool IsExtraWall1 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X , TreeLocation.Y - TraceAmount , TreeLocation.Z-TraceAmount) , TraceChannel);
			//	bool IsExtraWall2 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X , TreeLocation.Y + TraceAmount , TreeLocation.Z-TraceAmount) , TraceChannel);
			//	bool IsExtraWall3 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X - TraceAmount , TreeLocation.Y , TreeLocation.Z-TraceAmount) , TraceChannel);
			//	bool IsExtraWall4 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X + TraceAmount , TreeLocation.Y , TreeLocation.Z-TraceAmount) , TraceChannel);
			//	bool IsGround = GetWorld()->LineTraceTestByChannel(TreeLocation, FVector(TreeLocation.X, TreeLocation.Y, TreeLocation.Z- TraceAmount), TraceChannel);
			//	bool IsExtraWall = (IsExtraWall1 || IsExtraWall2 || IsExtraWall3 || IsExtraWall4) && !IsGround;
			//	OctreeRef->SetIsWall(IsExtraWall);
			//}
		}

		//땅 판단
		bool IsGround = GetWorld()->LineTraceTestByChannel(TreeLocation, FVector(TreeLocation.X, TreeLocation.Y, TreeLocation.Z- TraceAmount), TraceChannel);
		OctreeRef->SetIsGround(IsGround);
	}
	return IsFree;
}
