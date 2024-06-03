// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "HermesPlayerCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AHermesAIController;
struct FInputActionValue;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game,Abstract)
class AHermesPlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AHermesPlayerCharacter();

	bool IsLeader() const;
	const AHermesPlayerCharacter& GetLeader() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	bool IsGliding() const;

	UFUNCTION(BlueprintCallable)
	bool IsClimbing() const;

	UFUNCTION(BlueprintCallable)
	float GetClimbingYAxis() const;

	UFUNCTION(BlueprintCallable)
	void SetIsGliding(bool isGliding);



	UFUNCTION(BlueprintCallable)
	TArray<AHermesPlayerCharacter*> GetTeamCharacters() const;

	UFUNCTION(BlueprintCallable)
	const class UGA_Activatable* GetActivatableAbility(int32 index) const;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class AController* GetAIController() const { return AIController; }
	FORCEINLINE class AController* SetAIController(class AController* HermesAIController)  { return AIController = HermesAIController; }

	
	void Move(FVector Direction);
public:
	UPROPERTY()
	TWeakObjectPtr<AHermesPlayerCharacter> PreviousCharacter;

	UPROPERTY()
	TWeakObjectPtr<AHermesPlayerCharacter> NextCharacter;
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);


	UFUNCTION(BlueprintNativeEvent)
    void GliderInputPressed();
    virtual void GliderInputPressed_Implementation();

	void ClimbCancelInputPressed();


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
private:
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);
	void AutoAttack();
	bool FacingWallTrace(FHitResult& OutHitResult);
	void EnableClimbing(const FHitResult& HitResult);
	void DisableClimbing();
	bool CanClimbToTop() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Skill1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Skill2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Skill3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Skill4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ChangeTargetCWAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ChangeTargetCCWAction;

	UPROPERTY()
	TObjectPtr<AController> AIController;

	UPROPERTY()
 	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<class UHermesAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere,Category=GAS)
	TArray<TSubclassOf<class UGA_Activatable>> StartActivatableAbilities;

	UPROPERTY(EditAnywhere,Category=GAS)
	TSubclassOf<class UGA_Pathfinding> StartPathfindingAbility;

	UPROPERTY(EditAnywhere,Category=GAS)
	TSubclassOf<class UGA_AutoAttack> StartAutoAttackAbility;



	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTargetLockComponent> TargetLockComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> HPBar;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> ClimbToTopAnimMontage;


	bool bIsGliding;

	UPROPERTY(EditAnywhere)
	float GlidFallingSpeed = 100.f;

	UPROPERTY(EditAnywhere)
	float ClimbingSpeed = 0.2f;

	UPROPERTY(EditAnywhere)
	float ClimbToTopForward = 50.f;//벽타기에서 정상에 오르는 검사를 수행할시 사용될 Forward값

	UPROPERTY(EditAnywhere)
	float ClimbToTopUp = 150.f;//벽타기에서 정상에 오르는 검사를 수행할시 사용될 Up값


	bool bIsClimbing;



};

