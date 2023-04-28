// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Paladin.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Attack UMETA(DisplayName = "Attack"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SUMTEMPLERPG_API APaladin : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APaladin();

	FTimerHandle ChargeCoolTimer;

	FTimerHandle CastingCoolTimer;

	FTimerHandle ComboCoolTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkillComponent")
	class USkillComponent* SkillComponent;

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	bool bDied;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class APaladinPlayerController* PaladinPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintingStamina;

	float InterpSpeed;

	bool bInterpToEnemy;

	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	// 톰 루먼 강사는 protected에 선언하는 것이 권고사항이라 했었지만 public에 할당하고 있다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 Coins;

	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void IncrementCoin(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	void Die();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gage")
	float MaxGage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gage")
	float Gage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gage")
	float GageRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bChargeDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bChargeAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bCastingDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bCastingAttack;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	bool bMovingForward;

	bool bMovingRight;

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	virtual void Jump() override;

	void ShiftKeyDown();

	void ShiftKeyUp();

	bool bLMBDown;

	void LMBDown();

	void LMBUp();

	void ChargeDown();

	void ChargeUp();

	void GageUp();

	void CastingDown();

	void CastingUp();

	UFUNCTION(BlueprintCallable)
	void CastingSkill();

	UFUNCTION(BlueprintCallable)
	void CastingAttack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bComboKeyDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bComboSecond;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bJumpSecond;

	void ComboDown();

	void ComboUp();

	UFUNCTION(BlueprintCallable)
	void JumpPermission();

	UFUNCTION(BlueprintCallable)
	void ComboKeyEnd();

	bool bESCDown;

	void ESCDown();

	void ESCUp();

	FTimerHandle ChargeTimer;

	FTimerHandle CastingTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquipWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	void SetEquipWeapon(AWeapon* WeaponToSet);

	FORCEINLINE AWeapon* GetEquipWeapon() { return EquipWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	void LoadGameNoSwitch();

	UFUNCTION(BlueprintCallable)
	void SetChargeBegin();

	UFUNCTION(BlueprintCallable)
	void SetChargeEnd();

	UFUNCTION(BlueprintCallable)
	void SetCastingEnd();
};
