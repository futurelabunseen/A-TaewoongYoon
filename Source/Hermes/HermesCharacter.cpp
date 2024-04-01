// Copyright Epic Games, Inc. All Rights Reserved.

#include "HermesCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "GA_Activatable.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AHermesCharacter

AHermesCharacter::AHermesCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

bool AHermesCharacter::IsLeader() const
{
	return (nullptr != Cast<APlayerController>(GetController()));//AI컨트롤러가 아닌 플레이어 컨트롤러가 조종하는 캐릭터가 리더
}

const AHermesCharacter& AHermesCharacter::GetLeader() const
{
	if (IsLeader())
		return *this;
	check(NextCharacter.Get());
	return NextCharacter->GetLeader();//원형 연결리스트를 순회하며 리더찾기
}

UAbilitySystemComponent* AHermesCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

const UGA_Activatable* AHermesCharacter::GetActivatableAbility(int32 index) const
{
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
	AbilitySystemComponent->GetAllAbilities(AbilitySpecHandles);
	for ( const auto& AbilitySpecHandle : AbilitySpecHandles )
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilitySpecHandle);
		if ( AbilitySpec->InputID == index )
		{
			return CastChecked<UGA_Activatable>(AbilitySpec->Ability);
		}
	}
	check(nullptr);
	return nullptr;
}

void AHermesCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	int32 InputId = 1;
	for (const auto& StartAbility : StartActivatableAbilities)
	{//초기 Activatable GA부여(ID는 1,2,3,4)
		FGameplayAbilitySpec StartSpec(StartAbility);
		StartSpec.InputID = InputId++;
		AbilitySystemComponent->GiveAbility(StartSpec);
	}
}


void AHermesCharacter::GASInputPressed(int32 InputId)
{
	FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			AbilitySystemComponent->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			AbilitySystemComponent->TryActivateAbility(Spec->Handle);
		}
	}
}

void AHermesCharacter::GASInputReleased(int32 InputId)
{
	FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			AbilitySystemComponent->AbilitySpecInputReleased(*Spec);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AHermesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHermesCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHermesCharacter::Look);
		if ( IsValid(AbilitySystemComponent) )
		{//GAS인풋 설정
			EnhancedInputComponent->BindAction(Skill1Action, ETriggerEvent::Triggered, this, &AHermesCharacter::GASInputPressed,1);
			EnhancedInputComponent->BindAction(Skill1Action, ETriggerEvent::Completed, this, &AHermesCharacter::GASInputReleased,1);
			EnhancedInputComponent->BindAction(Skill2Action, ETriggerEvent::Triggered, this, &AHermesCharacter::GASInputPressed,2);
			EnhancedInputComponent->BindAction(Skill2Action, ETriggerEvent::Completed, this, &AHermesCharacter::GASInputReleased,2);
			EnhancedInputComponent->BindAction(Skill3Action, ETriggerEvent::Triggered, this, &AHermesCharacter::GASInputPressed,3);
			EnhancedInputComponent->BindAction(Skill3Action, ETriggerEvent::Completed, this, &AHermesCharacter::GASInputReleased,3);
			EnhancedInputComponent->BindAction(Skill4Action, ETriggerEvent::Triggered, this, &AHermesCharacter::GASInputPressed,4);
			EnhancedInputComponent->BindAction(Skill4Action, ETriggerEvent::Completed, this, &AHermesCharacter::GASInputReleased,4);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AHermesCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AHermesCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}