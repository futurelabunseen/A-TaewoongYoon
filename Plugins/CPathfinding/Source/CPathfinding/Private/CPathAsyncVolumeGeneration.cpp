// Copyright Dominik Trautman. Published in 2022. All Rights Reserved.


#include "CPathAsyncVolumeGeneration.h"
#include "CPathVolume.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Templates/Function.h"
#include "Engine/World.h"
#include "CPathCore.h"
#include <thread>

FCPathAsyncVolumeGenerator::FCPathAsyncVolumeGenerator(ACPathVolume* Volume, uint32 StartIndex, uint32 EndIndex, uint8 ThreadID, FString ThreadName, bool Obstacles)
	:
	FCPathAsyncVolumeGenerator(Volume)
{
	bObstacles = Obstacles;
	FirstIndex = StartIndex;
	LastIndex = EndIndex;
	GenThreadID = ThreadID;
	Name = ThreadName;
}

// Sets default values
FCPathAsyncVolumeGenerator::FCPathAsyncVolumeGenerator(ACPathVolume* Volume)
	:
	WakeUpCondition([this]() { return WakeUpCondition(); })
{
	VolumeRef = Volume;
	
}

FCPathAsyncVolumeGenerator::~FCPathAsyncVolumeGenerator()
{
	RequestedKill.store(true);
	if (ThreadRef)
		ThreadRef->Kill(true);
	ThreadRef = nullptr;
}

bool FCPathAsyncVolumeGenerator::Init()
{
	return true;
}

uint32 FCPathAsyncVolumeGenerator::Run()
{
	bIncreasedGenRunning = true;
	VolumeRef->GeneratorsRunning++;

	// Waiting for pathfinders to finish.
	// Generators have priority over pathfinders, so we block further pathfinders from starting by incrementing GeneratorsRunning first	
	while (!ShouldWakeUp())
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	
		
	if(RequestedKill.load())
		return 0;

#ifdef LOG_GENERATORS
	auto GenerationStart = TIMENOW;
#endif
	
	if (LastIndex > 0)
	{
		if (bObstacles)
		{//동적 장애물이 있는 경우(일부 voxel만 업데이트)
			auto StartIter = VolumeRef->TreesToRegenerate.begin();
			for (uint32 i = 0; i < FirstIndex; i++)
				StartIter++;

			auto EndIter = StartIter;
			for (uint32 i = FirstIndex; i < LastIndex; i++)
				EndIter++;

			for (auto Iter = StartIter; Iter != EndIter && !RequestedKill.load(); Iter++)
			{
				RefreshTree(*Iter);
			}
		}
		else
		{//초기 생성의 경우(전부 업데이트)
			for (uint32 OuterIndex = FirstIndex; OuterIndex < LastIndex && !RequestedKill.load(); OuterIndex++)
			{//OuterIndex: 초기 생성되는 x,y,z축 복셀의 곱 인덱스
				RefreshTree(OuterIndex);
			}
		}
	}

#ifdef LOG_GENERATORS
	auto GenerationTime = TIMEDIFF(GenerationStart, TIMENOW);

	int NodeCount = 0;
	for (int i = 0; i <= VolumeRef->OctreeDepth; i++)
	{
		NodeCount += OctreeCountAtDepth[i];
	}

	UE_LOG(LogTemp, Warning, TEXT("%s generated %d nodes in %lfms"), *Name, NodeCount, GenerationTime);
#endif

	if (bIncreasedGenRunning)
		VolumeRef->GeneratorsRunning--;
	bIncreasedGenRunning = false;
	return 0;
}

void FCPathAsyncVolumeGenerator::Stop()
{
	RequestedKill.store(true);

	// Preventing a potential deadlock if the process is somehow killed without waiting
	if (bIncreasedGenRunning)
		if(IsValid(VolumeRef))
			VolumeRef->GeneratorsRunning--;

	bIncreasedGenRunning = false;
}

void FCPathAsyncVolumeGenerator::Exit()
{
	ThreadExited.store(true);
}

bool FCPathAsyncVolumeGenerator::HasFinishedWorking()
{
	return ThreadExited.load();
}

void FCPathAsyncVolumeGenerator::RefreshTree(uint32 OuterIndex)
{
	CPathOctree* OctreeRef = &VolumeRef->Octrees[OuterIndex];
	if (!OctreeRef)
	{
		return;
	}
	RefreshTreeRec(OctreeRef, 0, VolumeRef->WorldLocationFromTreeID(OuterIndex));
}

FString FCPathAsyncVolumeGenerator::GetNameFromID(uint8 ID)
{
	return FString::Printf(TEXT("GeneratorThread %d"), (int)ID);
}

bool FCPathAsyncVolumeGenerator::RefreshTreeRec(CPathOctree* OctreeRef, uint32 Depth, FVector TreeLocation)
{

	bool IsFree = VolumeRef->RecheckOctreeAtDepth(OctreeRef, TreeLocation, Depth);

	OctreeCountAtDepth[Depth]++;

	//분석: RecheckOctreeAtDepth함수가 해당 octree노드가 free인지에 대한 정보를 저장해줌
	//      자식이 없는 노드는 leaf노드로 시각화될수 있음

	if (IsFree)
	{//빈 공간일때(자식이 존재하지 않음)
		delete[] OctreeRef->Children;
		OctreeRef->Children = nullptr;
		return true;
	}
	else if (++Depth <= (uint32)VolumeRef->OctreeDepth)
	{//primitive가 존재해서 Octree분기, depth가 한계 depth보다 작을때만 분기
		float HalfSize = VolumeRef->GetVoxelSizeByDepth(Depth) / 2.f;

		if (!OctreeRef->Children)
			OctreeRef->Children = new CPathOctree[8];
		uint8 FreeChildren = 0;
		// Checking children
		for (uint32 ChildIndex = 0; ChildIndex < 8; ChildIndex++)
		{//8개의 자식 분기
			FVector Location = TreeLocation + VolumeRef->LookupTable_ChildPositionOffsetMaskByIndex[ChildIndex] * HalfSize;
			FreeChildren += RefreshTreeRec(&OctreeRef->Children[ChildIndex], Depth, Location);
		}

		if (FreeChildren)
		{
			return true;
		}
		else
		{//자식중에 free인 자식이 하나도 없을때
			delete[] OctreeRef->Children;
			OctreeRef->Children = nullptr;
			return false;
		}

	}
	return false;
}

bool FCPathAsyncVolumeGenerator::ShouldWakeUp()
{
	return VolumeRef->PathfindersRunning.load() == 0 || RequestedKill.load();
}


