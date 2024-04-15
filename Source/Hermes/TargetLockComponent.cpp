// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetLockComponent.h"
#include "GameFramework\Character.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"


UTargetLockComponent::UTargetLockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

const AActor* UTargetLockComponent::GetTargetActor() const
{
    return TargetActor;
}

void UTargetLockComponent::ChangeTargetActorClockwise()
{//Target액터를 시계방향에 있는 다른 액터로 전환
    if ( bIsInputPressed )
        return;
    InputPressed();

    TArray<AActor*> OverlappingActorsInRadius;
    TSet<AActor*> OverlappingActors;
    ensure(OpponentCharacterClass);
	GetOverlappingActors(OverlappingActors,OpponentCharacterClass);
    for ( const auto& OverlappingActor : OverlappingActors )
    {
        FVector Vec = OverlappingActor->GetActorLocation() - GetAttachParentActor()->GetActorLocation();
        if ( Vec.Size() < SphereRadius )
        {
            OverlappingActorsInRadius.Emplace(OverlappingActor);
        }
    }

    FVector CurrentCharacterLocation = GetAttachParentActor()->GetActorLocation();
    FVector CurrentTargetDirection = FVector::ZeroVector;
    if ( TargetActor )
        CurrentTargetDirection = ( TargetActor->GetActorLocation() - CurrentCharacterLocation ).GetSafeNormal();
    else
        CurrentTargetDirection = GetAttachParentActor()->GetActorForwardVector();//Target액터가 Nullptr이라면 액터의 forward벡터를 이용
    float CurrentTargetAngle = FMath::RadiansToDegrees(atan2(CurrentTargetDirection.Y, CurrentTargetDirection.X));

    AActor* NextTarget = nullptr;
    float BestAngleDifference = 360.0f; // 최대 각도 차이

    for (AActor* PotentialTarget : OverlappingActorsInRadius)
    {
        if (PotentialTarget == TargetActor)
        {
            continue; // 현재 타겟은 건너뜀
        }

        FVector PotentialTargetDirection = (PotentialTarget->GetActorLocation() - CurrentCharacterLocation).GetSafeNormal();
        float PotentialTargetAngle = FMath::RadiansToDegrees(atan2(PotentialTargetDirection.Y, PotentialTargetDirection.X));

        float AngleDifference = FMath::Fmod(PotentialTargetAngle - CurrentTargetAngle + 360.0f, 360.0f);
        if (AngleDifference > 0 && AngleDifference < BestAngleDifference)
        {
            NextTarget = PotentialTarget;
            BestAngleDifference = AngleDifference;
        }
    }
    TargetActor = NextTarget;
}

void UTargetLockComponent::ChangeTargetActorCounterClockwise()
{//Target액터를 반시계방향에 있는 다른 액터로 전환
    if ( bIsInputPressed )
        return;
    InputPressed();

    TArray<AActor*> OverlappingActorsInRadius;
    TSet<AActor*> OverlappingActors;
    ensure(OpponentCharacterClass);
	GetOverlappingActors(OverlappingActors,OpponentCharacterClass);
    for ( const auto& OverlappingActor : OverlappingActors )
    {
        FVector Vec = OverlappingActor->GetActorLocation() - GetAttachParentActor()->GetActorLocation();
        if ( Vec.Size() < SphereRadius )
        {
            OverlappingActorsInRadius.Emplace(OverlappingActor);
        }
    }

    FVector CurrentCharacterLocation = GetAttachParentActor()->GetActorLocation();
    FVector CurrentTargetDirection = FVector::ZeroVector;
    if ( TargetActor )
        CurrentTargetDirection = ( TargetActor->GetActorLocation() - CurrentCharacterLocation ).GetSafeNormal();
    else
        CurrentTargetDirection = GetAttachParentActor()->GetActorForwardVector();//Target액터가 Nullptr이라면 액터의 forward벡터를 이용
    float CurrentTargetAngle = FMath::RadiansToDegrees(atan2(CurrentTargetDirection.Y, CurrentTargetDirection.X));

    AActor* NextTarget = nullptr;
    float LargestNegativeAngleDifference = -360.0f; // 가장 큰 음수 각도 차이

    for (AActor* PotentialTarget : OverlappingActorsInRadius)
    {
        if (PotentialTarget == TargetActor || PotentialTarget == nullptr)
        {
            continue;
        }

        FVector DirectionToPotentialTarget = (PotentialTarget->GetActorLocation() - CurrentCharacterLocation).GetSafeNormal();
        float PotentialTargetAngle = FMath::RadiansToDegrees(FMath::Atan2(DirectionToPotentialTarget.Y, DirectionToPotentialTarget.X));

        float AngleDifference = PotentialTargetAngle - CurrentTargetAngle;

        // 각도 차이를 -180 ~ 180 범위로 조정
        if (AngleDifference > 180) AngleDifference -= 360;
        if (AngleDifference <= -180) AngleDifference += 360;

        // 반시계방향으로 가장 가까운 타겟 찾기
        if (AngleDifference < 0 && AngleDifference > LargestNegativeAngleDifference)
        {
            NextTarget = PotentialTarget;
            LargestNegativeAngleDifference = AngleDifference;
        }
    }
    TargetActor = NextTarget;
}

void UTargetLockComponent::InputPressed()
{
    bIsInputPressed = true;
}

void UTargetLockComponent::InputReleased()
{
    bIsInputPressed = false;
}

void UTargetLockComponent::TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* a)
{
    Super::TickComponent(DeltaTime , TickType , a);
#if WITH_EDITOR
    FVector SphereLocation = GetComponentLocation();

    if ( GetNumOpponentCharacter() == 0 )
    {
        //DrawDebugSphere(
        //    GetWorld(),
        //    SphereLocation,
        //    SphereRadius,
        //    10, // 세그먼트 수, 더 많이 설정할수록 스피어가 더 부드러워집니다.
        //    FColor::Green, // 색상
        //    false, // 지속적으로 그릴 것인지 여부
        //    -1.0f, // 지속 시간
        //    0, // 깊이
        //    0.3f // 라인 두께
        //);
    }
    else
    {
        //DrawDebugSphere(
        //    GetWorld(),
        //    SphereLocation,
        //    SphereRadius,
        //    10, // 세그먼트 수, 더 많이 설정할수록 스피어가 더 부드러워집니다.
        //    FColor::Red, // 색상
        //    false, // 지속적으로 그릴 것인지 여부
        //    -1.0f, // 지속 시간
        //    0, // 깊이
        //    0.3f // 라인 두께
        //);
    }

#endif

}


void UTargetLockComponent::BeginPlay()
{
	Super::BeginPlay();
    //OnComponentBeginOverlap.AddDynamic(this , &UTargetLockComponent::HandleOverlapBegin);
    //OnComponentEndOverlap.AddDynamic(this , &UTargetLockComponent::HandleOverlapEnd);
}

void UTargetLockComponent::HandleOverlapBegin(
    UPrimitiveComponent* OverlappedComponent ,
    AActor* OtherActor ,
    UPrimitiveComponent* OtherComponent ,
    int32 OtherBodyIndex ,
    bool bFromSweep , 
    const FHitResult& SweepResult
)
{
    if ( TargetActor )
        return;
    if (UCapsuleComponent* OtherCapsule =  Cast<UCapsuleComponent>(OtherComponent) )
    {
        if ( AActor* OpponentActor = OtherCapsule->GetAttachmentRootActor())
        {
            if ( OpponentActor->GetClass()->IsChildOf(OpponentCharacterClass) )
            {
                TargetActor = OpponentActor;
                if ( GetAttachParentActor()->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()) )
                {//GAS액터라면 Targetting태그 부착
                    IAbilitySystemInterface* ASC = Cast<IAbilitySystemInterface>(GetAttachParentActor());
                    ASC->GetAbilitySystemComponent()->AddLooseGameplayTag(OnTargettingTag);
                }
            }
        }
    }
}

void UTargetLockComponent::HandleOverlapEnd(
    UPrimitiveComponent* OverlappedComponent ,
    AActor* OtherActor ,
    UPrimitiveComponent* OtherComponent ,
    int32 OtherBodyIndex
)
{
    if (UCapsuleComponent* OtherCapsule =  Cast<UCapsuleComponent>(OtherComponent) )
    {
        if ( OtherCapsule->GetAttachmentRootActor() == TargetActor )
        {
            TargetActor = nullptr;
            if ( GetAttachParentActor()->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()) )
            {//GAS액터라면 Targetting태그 해제
                IAbilitySystemInterface* ASC = Cast<IAbilitySystemInterface>(GetAttachParentActor());
                ASC->GetAbilitySystemComponent()->RemoveLooseGameplayTag(OnTargettingTag);
            }
        }
    }
}

int32 UTargetLockComponent::GetNumOpponentCharacter() const
{
    int32 NumOpponent = 0;
    TSet<AActor*> OverlappingActors;
    ensure(OpponentCharacterClass);
	GetOverlappingActors(OverlappingActors,OpponentCharacterClass);
    for ( const auto& OverlappingActor : OverlappingActors )
    {
        FVector Vec = OverlappingActor->GetActorLocation() - GetAttachParentActor()->GetActorLocation();
        if ( Vec.Size() < SphereRadius )
        {
            NumOpponent++;
        }
    }
    return NumOpponent;
}



