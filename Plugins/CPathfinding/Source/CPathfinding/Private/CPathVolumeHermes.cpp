// Fill out your copyright notice in the Description page of Project Settings.


#include "CPathVolumeHermes.h"
#include "CPathFindPath.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

ACPathVolumeHermes::ACPathVolumeHermes()
{
	HISMComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISMComponent"));
}

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
	GenerationCompleteDelegate.AddDynamic(this , &ACPathVolumeHermes::SpawnMinimapVoxel);
	GenerationCompleteDelegate.AddDynamic(this , &ACPathVolumeHermes::SpawnMinimapCamera);
	Super::BeginPlay();//volume안에 voxel들 생성
}

void ACPathVolumeHermes::CalcFitness(CPathAStarNode& Node , FVector TargetLocation , int32 UserData)
{
	check(Node.PreviousNode);

	Node.DistanceSoFar = Node.PreviousNode->DistanceSoFar + FVector::Distance(Node.PreviousNode->WorldLocation, Node.WorldLocation);//현재노드까지 거리누적
	
	
	if (ExtractIsGroundFromData(Node.TreeUserData))
	{
		Node.DistanceSoFar += 180;
	}
	if (ExtractIsWallFromData(Node.TreeUserData))
	{
		Node.DistanceSoFar += 100;
	}
	
	Node.FitnessResult = Node.DistanceSoFar + 3.5f * FVector::Distance(Node.WorldLocation, TargetLocation);//FitnessResult: A*평가값, 낮을수록 우선순위
}

bool ACPathVolumeHermes::RecheckOctreeAtDepth(CPathOctree* OctreeRef , FVector TreeLocation , uint32 Depth)
{
	bool IsFree = Super::RecheckOctreeAtDepth(OctreeRef, TreeLocation, Depth);
	if (IsFree)
	{
		float TraceAmount = VoxelSize * 1.49f;//VoxelSize: 최소복셀 크기
		{//벽 판단
			bool IsWall1 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X , TreeLocation.Y - TraceAmount , TreeLocation.Z) , TraceChannel);
			bool IsWall2 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X , TreeLocation.Y + TraceAmount , TreeLocation.Z) , TraceChannel);
			bool IsWall3 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X - TraceAmount , TreeLocation.Y , TreeLocation.Z) , TraceChannel);
			bool IsWall4 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X + TraceAmount , TreeLocation.Y , TreeLocation.Z) , TraceChannel);
			bool IsWall = IsWall1 || IsWall2 || IsWall3 || IsWall4;
			OctreeRef->SetIsWall(IsWall);

			if ( !IsWall )
			{//벽은 아니지만 대각선 아래방향에 벽이 존재하는 경우 -> 벽으로 취급
				bool IsExtraWall1 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X , TreeLocation.Y - TraceAmount , TreeLocation.Z-TraceAmount) , TraceChannel);
				bool IsExtraWall2 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X , TreeLocation.Y + TraceAmount , TreeLocation.Z-TraceAmount) , TraceChannel);
				bool IsExtraWall3 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X - TraceAmount , TreeLocation.Y , TreeLocation.Z-TraceAmount) , TraceChannel);
				bool IsExtraWall4 = GetWorld()->LineTraceTestByChannel(TreeLocation , FVector(TreeLocation.X + TraceAmount , TreeLocation.Y , TreeLocation.Z-TraceAmount) , TraceChannel);
				bool IsGround = GetWorld()->LineTraceTestByChannel(TreeLocation, FVector(TreeLocation.X, TreeLocation.Y, TreeLocation.Z- TraceAmount), TraceChannel);
				bool IsExtraWall = (IsExtraWall1 || IsExtraWall2 || IsExtraWall3 || IsExtraWall4) && !IsGround;
				OctreeRef->SetIsWall(IsExtraWall);
			}
		}

		//땅 판단
		bool IsGround = GetWorld()->LineTraceTestByChannel(TreeLocation, FVector(TreeLocation.X, TreeLocation.Y, TreeLocation.Z- TraceAmount), TraceChannel);
		OctreeRef->SetIsGround(IsGround);
	}
	
	return IsFree;
}

void ACPathVolumeHermes::SpawnMinimapCamera()
{
	check(MinimapRenderTarget);
	
	ASceneCapture2D* MinimapSceneCapture2D = GetWorld()->SpawnActor<ASceneCapture2D>(
		ASceneCapture2D::StaticClass(),
		GetActorLocation() + FVector(VolumeBox->GetScaledBoxExtent().X,0,-VolumeBox->GetScaledBoxExtent().Z) ,
		FRotator(-45,-180,0)
	);
	if ( MinimapSceneCapture2D )
	{
		// Access the SceneCaptureComponent2D
        USceneCaptureComponent2D* CaptureComponent = MinimapSceneCapture2D->GetCaptureComponent2D();
        if (CaptureComponent)
        {
            // Set the render target
            CaptureComponent->TextureTarget = MinimapRenderTarget;
            CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
            CaptureComponent->bCaptureEveryFrame = true;

			// Disable shadows in the capture component
            CaptureComponent->ShowFlags.DynamicShadows = false;
        }
	}
}

void ACPathVolumeHermes::SpawnMinimapVoxel()
{
	//    0. Voxel 멀티쓰레드 생성작업 wait(delegate를 이용해서 대기)
	//    1. Octree포인터 Get(this->Octrees)
	//    2. OuterIndex loop돌기(NodeCount[3] 배열 이용)
	//    3. 만약 해당 Octree가 벽이나 바닥일시 SpawnMinimapStaticMesh호출
	uint32 OuterNodeCount = NodeCount[0] * NodeCount[1] * NodeCount[2];//OuterNodeCount: 0depth인 voxel의 개수
	ensure(MinimapVoxelMesh);
	HISMComponent->SetStaticMesh(MinimapVoxelMesh);
	HISMComponent->SetMaterial(0, MinimapMaterial);
	for (uint32 i = 0; i < OuterNodeCount; i++) 
	{
		FTransform InstanceTransform;
		CPathOctree* Octree = &Octrees[i];
		if ( !Octree->GetIsFree() )
		{
			FVector VoxelLocation = WorldLocationFromTreeID(i);
			FVector VoxelLocationOffset = FVector(0 , 0 , -VolumeBox->GetScaledBoxExtent().Z * 2.f);
			InstanceTransform.SetLocation(VoxelLocation + VoxelLocationOffset);
			InstanceTransform.SetScale3D(FVector(VoxelSize / 100.f));
			HISMComponent->AddInstance(InstanceTransform);
		}
	}


}
