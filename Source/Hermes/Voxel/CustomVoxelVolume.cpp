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
	bool IsFree = true;
	for (const auto& Shape : TraceShapesByDepth[Depth])
	{
		if (GetWorld()->OverlapAnyTestByChannel(TreeLocation, FQuat(FRotator(0)), TraceChannel, Shape))
		{//아무 액터나 충돌시
			TArray<FOverlapResult> OverlapResults;
			if (GetWorld()->OverlapMultiByChannel(OverlapResults, TreeLocation, FQuat(FRotator(0)),TraceChannel, Shape))
			{
				IsFree = false;
				for (const auto& OverlapResult : OverlapResults )
				{
					if ( UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(OverlapResult.GetComponent()) )
					{
						if ( MeshComponent->GetStaticMesh()->GetFName() == WallMesh->GetFName() )
						{//설정한 WallMesh일시 
							OctreeRef->SetIsWall(true);
							break;
						}
					}
				}
				break;
			}
		}
	}
	OctreeRef->SetIsFree(IsFree);
	return IsFree;
	
}


void ACustomVoxelVolume::BeginPlay()
{
	Super::BeginPlay();//부모 BeginPlay함수에서 복셀 생성
}
