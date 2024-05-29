// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Pathfinding.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Runtime/NavigationSystem/Public/NavigationPath.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "CPathVolumeHermes.h"
#include "Kismet/GameplayStatics.h"

UGA_Pathfinding::UGA_Pathfinding()
	:
	GoalLocation(FVector::ZeroVector),
	MaxNumOfSplinePoints(60),
	SplineTickness(0.1f),
	RouteZHeight(40.f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Pathfinding::CancelAbility(const FGameplayAbilitySpecHandle Handle , const FGameplayAbilityActorInfo* ActorInfo , const FGameplayAbilityActivationInfo ActivationInfo , bool bReplicateCancelAbility)
{//Spline Component를 Avatar Actor에서 제거
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	USplineComponent* SplineComponent = AvatarActor->GetComponentByClass<USplineComponent>();
	check(SplineComponent);
	
	SplineComponent->UnregisterComponent();
	SplineComponent->DestroyComponent();

	TArray<USplineMeshComponent*> SplineMeshComponents;
	GetAvatarActorFromActorInfo()->GetComponents<USplineMeshComponent>(SplineMeshComponents);
	check(SplineMeshComponents.Num() != 0);

	for ( const auto& SplineMeshComponent : SplineMeshComponents )
	{
		SplineMeshComponent->UnregisterComponent();
		SplineMeshComponent->DestroyComponent();
	}
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Pathfinding::ActivateAbility(const FGameplayAbilitySpecHandle Handle , const FGameplayAbilityActorInfo* ActorInfo , const FGameplayAbilityActivationInfo ActivationInfo , const FGameplayEventData* TriggerEventData)
{//VoxelVolume참조 초기화 + Spline Component를 Avatar Actor에 부착

	TArray<AActor*> PathVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld() , ACPathVolume::StaticClass() , PathVolumes);
	check(PathVolumes.Num() > 0);
	for ( const auto& PathVolume : PathVolumes )
	{
		ACPathVolumeHermes* PathVolumeHermes = CastChecked<ACPathVolumeHermes>(PathVolume);
		VoxelVolume = PathVolumeHermes;
		break;
	}



	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	USplineComponent* SplineComponent = NewObject<USplineComponent>(AvatarActor , USplineComponent::StaticClass());
	check(SplineComponent);
	SplineComponent->bDrawDebug = true;
	SplineComponent->RegisterComponent();
	
	for ( uint16 i = 0; i < MaxNumOfSplinePoints; i++ )
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(AvatarActor , USplineMeshComponent::StaticClass());
		check(SplineMeshComponent);
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->SetStaticMesh(SplineStaticMesh);
		SplineMeshComponent->SetMaterial(0,AirRouteMaterial);
		SplineMeshComponent->SetForwardAxis(ESplineMeshAxis::Y);
		SplineMeshComponent->SetStartScale(FVector2D(SplineTickness,SplineTickness));
		SplineMeshComponent->SetEndScale(FVector2D(SplineTickness,SplineTickness));
		SplineMeshComponent->RegisterComponent();
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

const TArray<FVector>& UGA_Pathfinding::FindNavMeshPathSync()
{
	const UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	const UNavigationPath* path = navSys->FindPathToLocationSynchronously(
		GetWorld(),
		GetAvatarActorFromActorInfo()->GetActorLocation(),
		GoalLocation
	);
	return path->PathPoints;
}

void UGA_Pathfinding::UpdatePath(const TArray<FVector>& PathPoints)
{
	USplineComponent* SplineComponent = GetAvatarActorFromActorInfo()->GetComponentByClass<USplineComponent>();
	{//Spline 초기화 후 path정보에 따라 새로 업데이트
		check(SplineComponent);
		SplineComponent->ClearSplinePoints();
		for(uint8 i = 0;i<PathPoints.Num();i++)
		{
			SplineComponent->AddSplinePoint(PathPoints[i] + FVector(0.0,0.0,RouteZHeight),ESplineCoordinateSpace::World);
		}
	}
	{//업데이트된 Spline정보로 Spline Mesh구성
		TArray<USplineMeshComponent*> SplineMeshComponents;
		GetAvatarActorFromActorInfo()->GetComponents<USplineMeshComponent>(SplineMeshComponents);
		check(SplineMeshComponents.Num() != 0);
		for(uint8 i = 0;i<MaxNumOfSplinePoints-1;i++)
		{
			SplineMeshComponents[i]->SetVisibility(false);
		}
		for(uint8 i = 0u;i<SplineComponent->GetNumberOfSplineSegments();i++)
		{//Spline구역 Loop
			FVector splineStartPointLocation(FVector::Zero());
			FVector splineStartPointTangent(FVector::Zero());
			SplineComponent->GetLocationAndTangentAtSplinePoint(i,splineStartPointLocation,splineStartPointTangent,ESplineCoordinateSpace::Local);


			FVector splineEndPointLocation(FVector::Zero());
			FVector splineEndPointTangent(FVector::Zero());
			SplineComponent->GetLocationAndTangentAtSplinePoint(i+1,splineEndPointLocation,splineEndPointTangent,ESplineCoordinateSpace::Local);

			

			SplineMeshComponents[i]->SetStartAndEnd(
				splineStartPointLocation,
				splineStartPointTangent,
				splineEndPointLocation,
				splineEndPointTangent,
				true
			);
			switch ( VoxelVolume->GetVoxelType((splineStartPointLocation + splineEndPointLocation) /2) )
			{
			case 0:
				SplineMeshComponents[i]->SetMaterial(0 , GroundRouteMaterial);
				break;
			case 1:
				SplineMeshComponents[i]->SetMaterial(0 , WallRouteMaterial);
				break;
			case 2:
				SplineMeshComponents[i]->SetMaterial(0 , AirRouteMaterial);
				break;
			}
			
			
			SplineMeshComponents[i]->SetVisibility(true);
		}
		
	}
}

void UGA_Pathfinding::SetGoal(FVector Goal)
{
	GoalLocation = Goal;
}

TArray<FVector> UGA_Pathfinding::ConvertCPathNode(const TArray<FCPathNode>& CPathNodes)
{
	TArray<FVector> Result;
	Result.Reserve(CPathNodes.Num());
	for ( const auto CPathNode : CPathNodes )
	{
		Result.Emplace(CPathNode.WorldLocation);
	}
	return Result;
}
