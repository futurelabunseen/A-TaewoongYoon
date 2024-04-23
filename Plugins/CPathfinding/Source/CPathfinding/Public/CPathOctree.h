// Copyright Dominik Trautman. Published in 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */



 // The Octree representation
class CPATHFINDING_API CPathOctree
{
public:
	CPathOctree();


	CPathOctree* Children = nullptr;

	uint32 Data = 0;


	inline void SetIsFree(bool IsFree)
	{
		Data &= 0xFFFFFFFE;
		Data |= (uint32)IsFree;
	}

	inline bool GetIsFree() const
	{
		return Data << 31;
	}


	inline void SetIsWall(bool IsWall)
	{
		Data &= 0xFFFFFFFD;//11111111111111111111111111111101(2비트 값 날리기)
		Data |= (IsWall << 1);//0~~~00000010(2비트값 1로 설정)
	}

	inline bool GetIsWall() const
	{
		return Data & 0x00000002;
	}

	inline void SetIsGround(bool IsGround)
	{
		Data &= 0xFFFFFFFB;//11111111111111111111111111111011(3비트 값 날리기)
		Data |= (IsGround << 2);//0~~~00000100(3비트값 1로 설정)
	}

	inline bool GetIsGround() const
	{
		return Data & 0x00000004;
	}


	~CPathOctree()
	{
		delete[] Children;
	};
};

// Class used to remember data needed to draw a debug voxel 
class CPATHFINDING_API CPathVoxelDrawData
{

public:

	CPathVoxelDrawData()
	{}

	CPathVoxelDrawData(FVector WorldLocation, float VoxelExtent, bool IsFree)
		:
		Location(WorldLocation),
		Extent(VoxelExtent),
		Free(IsFree)
	{}

	FVector Location;
	float Extent;
	bool Free = false;

};

