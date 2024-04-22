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
						{//설정한 WallMesh일시 Wall데이터 부여(플러그인 제작자 방식대로 Data비트연산 사용)
							uint32 IsWall = true;
							OctreeRef->Data &= 0xFFFFFFFD;//11111111111111111111111111111101(2비트 값 날리기)
							OctreeRef->Data |= (IsWall << 1);//0~~~00000010(2비트값 1로 설정
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
