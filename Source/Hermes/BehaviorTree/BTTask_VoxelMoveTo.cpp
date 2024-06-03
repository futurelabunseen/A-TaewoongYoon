// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BTTask_VoxelMoveTo.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/HermesPlayerCharacter.h"
#include "InputActionValue.h"

UBTTask_VoxelMoveTo::UBTTask_VoxelMoveTo()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_VoxelMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory)
{
	UE_LOG(LogTemp , Log , TEXT("Voxel Move To execute"));
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	check(Pawn);
	
	UBlackboardComponent* Blackboard  =  Pawn->GetController()->FindComponentByClass<UBlackboardComponent>();
	check(Blackboard);
	
	FBT_VoxelMoveTo* TaskMemory = reinterpret_cast<FBT_VoxelMoveTo*>(NodeMemory);
	if ( !TaskMemory )
		check(false);
	TaskMemory->GoalLocation = Blackboard->GetValueAsVector(GoalLocationKey.SelectedKeyName);

	TSet<AActor*> PathVolumes;
	Pawn->GetOverlappingActors(PathVolumes , ACPathVolume::StaticClass());
	check(PathVolumes.Num() > 0);
	ACPathVolume* OverlappedPathVolume = CastChecked<ACPathVolume>(*PathVolumes.begin());
	TaskMemory->PathResult = OverlappedPathVolume->FindPathSynchronous(Pawn->GetActorLocation() , TaskMemory->GoalLocation , 0 , 0 , 1.f);
	TaskMemory->CurrentIndex = 0u;
	



	return EBTNodeResult::InProgress;
}

uint16 UBTTask_VoxelMoveTo::GetInstanceMemorySize() const
{
	return sizeof(FBT_VoxelMoveTo);//NodeMemory의 크기 지정
}

void UBTTask_VoxelMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds)
{
	Super::TickTask(OwnerComp , NodeMemory , DeltaSeconds);

	FBT_VoxelMoveTo* TaskMemory = reinterpret_cast<FBT_VoxelMoveTo*>(NodeMemory);
	if ( !TaskMemory )
		check(false);

	if ( TaskMemory->PathResult.UserPath.Num() == 0 )
	{//경로가 존재하지 않을때는 task종료
		FinishLatentTask(OwnerComp , EBTNodeResult::Succeeded);
		return;
	}

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	check(Pawn);
	
	
	//{//임시코드: CharacterMovement상태를 flying처리해서 중력영향받지 않게만듦
	//	UCharacterMovementComponent* CharacterMovementComponent = CastChecked<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	//	CharacterMovementComponent->GravityScale = 0.f;
	//	CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
	//}

	// 현재 index에서 다음 index 방향으로 이동,
	//       이동완료시 index++
	MoveAlongPath(Pawn,TaskMemory->PathResult.UserPath,TaskMemory->CurrentIndex,DeltaSeconds);
	//UE_LOG(LogTemp , Log , TEXT("Move route %d by %d") , TaskMemory->CurrentIndex , TaskMemory->PathResult.UserPath.Num());;
	if (TaskMemory->CurrentIndex >=  TaskMemory->PathResult.UserPath.Num())
    {//목적지 도달
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }


	//FinishLatentTask(OwnerComp , EBTNodeResult::Succeeded);//본 태스크의 상태변경(->Succeeded)
}

void UBTTask_VoxelMoveTo::MoveAlongPath(APawn* Pawn , const TArray<FCPathNode>& PathPoints , uint16& CurrentIndex , float DeltaSeconds)
{
	if ( CurrentIndex >= PathPoints.Num() )
		return;
    
    FVector CurrentLocation = Pawn->GetActorLocation();
    FVector TargetLocation = PathPoints[CurrentIndex].WorldLocation;
    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
        
    float Distance = FVector::Dist(CurrentLocation,TargetLocation);
    float Amount = 600.f;


    if (Distance < Amount)
    {//목표 index 도달 판정

        CurrentIndex++;
    }
    else
    {
		AHermesPlayerCharacter* HermesCharacter = Cast<AHermesPlayerCharacter>(Pawn);
		HermesCharacter->Move(Direction);
        
    }
    
}
