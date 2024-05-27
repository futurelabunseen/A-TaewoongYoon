// Fill out your copyright notice in the Description page of Project Settings.


#include "CPathVolumeHermes.h"
#include "CPathFindPath.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

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
	//voxel octree를 순회하며 3D 미니맵을 구성하는 static mesh스폰하는 함수 등록
	GenerationCompleteDelegate.BindUObject(this , &ACPathVolumeHermes::SpawnMinimapVoxel);
	Super::BeginPlay();//volume안에 voxel들 생성
}

void ACPathVolumeHermes::CalcFitness(CPathAStarNode& Node , FVector TargetLocation , int32 UserData)
{
	check(Node.PreviousNode);

	Node.DistanceSoFar = Node.PreviousNode->DistanceSoFar + FVector::Distance(Node.PreviousNode->WorldLocation, Node.WorldLocation);//현재노드까지 거리누적
	
	
	if (ExtractIsGroundFromData(Node.TreeUserData))
	{
		Node.DistanceSoFar += 700;
	}
	
	Node.FitnessResult = Node.DistanceSoFar + 3.5f * FVector::Distance(Node.WorldLocation, TargetLocation);//FitnessResult: A*평가값, 낮을수록 우선순위
}

bool ACPathVolumeHermes::RecheckOctreeAtDepth(CPathOctree* OctreeRef , FVector TreeLocation , uint32 Depth)
{
	bool IsFree = Super::RecheckOctreeAtDepth(OctreeRef, TreeLocation, Depth);
	if (IsFree)
	{
		float TraceAmount = VoxelSize * 1.49f;
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

void ACPathVolumeHermes::SpawnMinimapVoxel()
{
	//    0. Voxel 멀티쓰레드 생성작업 wait(delegate를 이용해서 대기)
	//    1. Octree포인터 Get(this->Octrees)
	//    2. OuterIndex loop돌기(NodeCount[3] 배열 이용)
	//    3. 만약 해당 Octree가 벽이나 바닥일시 SpawnMinimapStaticMesh호출
	uint32 OuterNodeCount = NodeCount[0] * NodeCount[1] * NodeCount[2];//OuterNodeCount: 0depth인 voxel의 개수
	for ( uint32 i = 0; i < OuterNodeCount; i++ )
	{
		CPathOctree* Octree = &Octrees[i];
		if ( Octree->GetIsGround() || Octree->GetIsWall() )
		{
			FVector VoxelLocation = WorldLocationFromTreeID(i);
			SpawnMinimapStaticMesh(VoxelLocation , GetActorRotation());
		}
	}
}

void ACPathVolumeHermes::SpawnMinimapStaticMesh(FVector Location,FRotator Rotation)
{
	ensure(MinimapVoxelMesh);
	 //StaticMesh액터 스폰
    AStaticMeshActor* NewMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(), Location, Rotation);



    //StaticMesh적용
    if (NewMeshActor)
    {
        UStaticMeshComponent* MeshComponent = NewMeshActor->GetStaticMeshComponent();
        if (MeshComponent)
        {
			MeshComponent->SetMobility(EComponentMobility::Movable);
			MeshComponent->SetWorldScale3D(FVector(VoxelSize / 100.0f));//VoxelSize에 맞게 스케일 적용
            MeshComponent->SetStaticMesh(MinimapVoxelMesh);
			MeshComponent->SetMobility(EComponentMobility::Static);
        }
    }
}
