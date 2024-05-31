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
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "Kismet/GameplayStatics.h"


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
	Super::BeginPlay();
	//voxel octree를 순회하며 3D 미니맵을 구성하는 static mesh스폰하는 함수 등록
	GenerationCompleteDelegate.AddDynamic(this , &ACPathVolumeHermes::SpawnMinimapVoxel);
	GenerationCompleteDelegate.AddDynamic(this , &ACPathVolumeHermes::SpawnMinimapCamera);

	if ( bUsingBakedData )
	{//baking된 복셀데이터가 있다면 loading,없다면 복셀 생성후 복셀데이터 baking
		if ( !LoadVoxel() )
		{
			GenerationCompleteDelegate.AddDynamic(this , &ACPathVolumeHermes::BakeVoxel);
			GenerateGraph();
			
		}
	}
	else
	{
		GenerateGraph();
	}
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
	MinimapSceneCapture2D->SetActorLocation(MinimapSceneCapture2D->GetActorLocation() + MinimapSceneCapture2D->GetActorForwardVector() * 5000.f);
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

void ACPathVolumeHermes::BakeVoxel()
{
	FString DirectoryPath = FPaths::ProjectDir() + TEXT("BakedData/");
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    
    if (!PlatformFile.DirectoryExists(*DirectoryPath))
    {//BakedData디렉토리가 없다면
        PlatformFile.CreateDirectory(*DirectoryPath);//생성

        if (!PlatformFile.DirectoryExists(*DirectoryPath))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create directory: %s"), *DirectoryPath);
            return;
        }
    }

	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	FString FilePath = FPaths::ProjectDir() + TEXT("BakedData/") + CurrentLevelName + TEXT(".txt");
    FString DataString;


	DataString.Append(FString::FromInt(NodeCount[0]) + TEXT("\n"));
	DataString.Append(FString::FromInt(NodeCount[1]) + TEXT("\n"));
	DataString.Append(FString::FromInt(NodeCount[2]) + TEXT("\n"));
	uint32 OuterNodeCount = NodeCount[0] * NodeCount[1] * NodeCount[2];
    for (uint32 i = 0u;i<OuterNodeCount;i++)
    {
        DataString.Append(FString::Printf(TEXT("%u"),Octrees[i].Data) + TEXT("\n"));
    }

    FFileHelper::SaveStringToFile(DataString, *FilePath);

}

bool ACPathVolumeHermes::LoadVoxel()
{
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	FString FilePath = FPaths::ProjectDir() + TEXT("BakedData/") + CurrentLevelName + TEXT(".txt");
    FString DataString;
	

	if ( FFileHelper::LoadFileToString(DataString , *FilePath) )
	{
		TArray<FString> DataLines;
		DataString.ParseIntoArrayLines(DataLines);

		for ( int32 i = 0; i < 3; i++ )
		{//x,y,z 복셀개수 load
			NodeCount[i] = FCString::Atoi(*DataLines[i]);
		}
		uint32 OuterNodeCount = NodeCount[0] * NodeCount[1] * NodeCount[2];
		Octrees = new CPathOctree[OuterNodeCount];
		for ( int32 i = 3; i < DataLines.Num(); i++ )
		{
			Octrees[i - 3].Data = static_cast<uint32>(FCString::Strtoui64(*DataLines[i] , nullptr , 10));
			Octrees[i - 3].Children = nullptr;
		}
		StartPosition = GetActorLocation() - VolumeBox->GetScaledBoxExtent() + GetVoxelSizeByDepth(0) / 2;
		for (int i = 0; i <= OctreeDepth; i++)
		{
			LookupTable_VoxelSizeByDepth[i] = VoxelSize * FMath::Pow(2.f, OctreeDepth - i);
			TraceShapesByDepth.emplace_back();
			TraceShapesByDepth.back().push_back(FCollisionShape::MakeBox(FVector(GetVoxelSizeByDepth(i) / 2.f)));

			float CurrSize = GetVoxelSizeByDepth(i);
			if (AgentRadius * 2 > CurrSize || AgentHalfHeight * 2 > CurrSize)
			{
				switch (AgentShape)
				{
				case Capsule:
					TraceShapesByDepth.back().push_back(FCollisionShape::MakeCapsule(AgentRadius, AgentHalfHeight));
				case Box:
					TraceShapesByDepth.back().push_back(FCollisionShape::MakeBox(FVector(AgentRadius, AgentRadius, AgentHalfHeight)));
				case Sphere:
					TraceShapesByDepth.back().push_back(FCollisionShape::MakeSphere(AgentRadius));
				default:
					break;
				}
			}
		}
		GeneratorsRunning.store(0);
		InitialGenerationCompleteAtom.store(true);
		InitialGenerationFinished = true;
		if (GenerationCompleteDelegate.IsBound())
		{//voxel생성후 수행하게되는 delegate존재시 호출
			GenerationCompleteDelegate.Broadcast();
		}
		return true;
	}
	else
		return false;

}
