// Fill out your copyright notice in the Description page of Project Settings.


#include "HermesNonPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Attribute\HermesAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "TargetLockComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AHermesNonPlayerCharacter::AHermesNonPlayerCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
	AttributeSet = CreateDefaultSubobject<UHermesAttributeSet>(TEXT("Attribute Set"));
	TargetLockComponent = CreateDefaultSubobject<UTargetLockComponent>(TEXT("TargetLock Component"));
	TargetLockComponent->SetupAttachment(RootComponent);


	HPBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP Bar"));
	HPBar->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UAbilitySystemComponent* AHermesNonPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AHermesNonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHermesNonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


