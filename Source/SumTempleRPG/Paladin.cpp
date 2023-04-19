// Fill out your copyright notice in the Description page of Project Settings.


#include "Paladin.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "ItemStorage.h"
#include "PaladinAnimInstance.h"
#include "PaladinPlayerController.h"
#include "STRSaveGame.h"

// Sets default values
APaladin::APaladin()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	GetCapsuleComponent()->SetCapsuleSize(22.f, 88.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;

	StaminaDrainRate = 25.f;
	MinSprintingStamina = 50.f;

	bLMBDown = false;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	bDied = false;

	bMovingForward = false;
	bMovingRight = false;

	bESCDown = false;

	MaxGage = 50.f;
	Gage = 0.f;
	GageRate = 10.f;

	bChargeDown = false;
	bChargeAttack = false;
}

void APaladin::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;

	if(MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void APaladin::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

FRotator APaladin::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LooAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LooAtRotationYaw;
}

void APaladin::DecrementHealth(float Amount)
{
	Health -= Amount;
	if(Health <= 0.f)
	{
		Health = 0.f;
		Die();
	}
}

float APaladin::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.f)
	{
		Die();

		if(DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);

			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}

	return DamageAmount;
}

void APaladin::IncrementCoin(int32 Amount)
{
	Coins += Amount;
}

void APaladin::IncrementHealth(float Amount)
{
	if(Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
}

void APaladin::Die()
{
	if(bDied)
	{
		return;
	}
	if(EquipWeapon)
	{
		EquipWeapon->Destroy();
	}

	DisableInput(PaladinPlayerController);
	SetMovementStatus(EMovementStatus::EMS_Dead);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}	
}

// Called when the game starts or when spawned
void APaladin::BeginPlay()
{
	Super::BeginPlay();

	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	PaladinPlayerController = Cast<APaladinPlayerController>(GetController());

	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if(Map != "SunTemple")
	{
		if (PaladinPlayerController)
		{
			PaladinPlayerController->GameModeOnly();
		}
	}
}

// Called every frame
void APaladin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(MovementStatus == EMovementStatus::EMS_Dead)
	{
		return;
	}

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if(bShiftKeyDown)
		{
			Stamina -= DeltaStamina;
			if(Stamina <= MinSprintingStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else
		{	
			if(Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if(bShiftKeyDown)
		{
			if(Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
		}
		else
		{
			Stamina += DeltaStamina;
			if(Stamina + DeltaStamina >= MinSprintingStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if(bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if(Stamina + DeltaStamina >= MinSprintingStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		Stamina += DeltaStamina;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default :
		;
	}

	if(bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if(CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();

		if(PaladinPlayerController)
		{
			PaladinPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

	if(EquipWeapon && bChargeDown)
	{
		EquipWeapon->ChargeDamage = Gage;
	}
}

// Called to bind functionality to input
void APaladin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); // if(PlayerInputComponent) �� ���� ����̴�. �Ҵ��� �ȵ� �ִٸ� �Լ��� �����Ų��.

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APaladin::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APaladin::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APaladin::ShiftKeyUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &APaladin::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &APaladin::ESCUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &APaladin::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &APaladin::LMBUp);

	PlayerInputComponent->BindAction("Charge", IE_Pressed, this, &APaladin::ChargeDown);
	PlayerInputComponent->BindAction("Charge", IE_Released, this, &APaladin::ChargeUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &APaladin::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APaladin::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APaladin::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APaladin::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &APaladin::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APaladin::LookUpAtRate);
}

void APaladin::MoveForward(float Value)
{
	bMovingForward = false;
	if(Controller && Value != 0.0f && (!bAttacking))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}

void APaladin::MoveRight(float Value)
{
	bMovingRight = false;

	if (Controller && Value != 0.0f && (!bAttacking))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true;
	}
}

void APaladin::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APaladin::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APaladin::Jump()
{
	if(!bAttacking)
	{
		Super::Jump();
	}
}

void APaladin::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void APaladin::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void APaladin::LMBDown()
{
	bLMBDown = true;

	if(ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);

		if(Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if(EquipWeapon)
	{
		if(!GetMovementComponent()->IsFalling())
		{
			Attack();
		}
	}
}

void APaladin::LMBUp()
{
	bLMBDown = false;
}

void APaladin::ChargeDown()
{
	if(EquipWeapon && !bChargeDown)
	{
		bChargeDown = true;
		PaladinPlayerController->DisplaySkillGage();

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 0.2f);
			AnimInstance->Montage_JumpToSection(FName("ReadyCharge"), CombatMontage);

			GetWorldTimerManager().SetTimer(ChargeTimer, this, &APaladin::GageUp, 0.5f, true, 0.0f);
		}
	}
}

void APaladin::ChargeUp()
{
	if(EquipWeapon && bChargeDown)
	{
		PaladinPlayerController->RemoveSkillGage();
		GetWorldTimerManager().ClearTimer(ChargeTimer);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.8f);
			AnimInstance->Montage_JumpToSection(FName("ChargeAttack"), CombatMontage);
		}
		bChargeDown = false;
	}
}

void APaladin::GageUp()
{
	if(bChargeDown &&  MaxGage >= Gage)
	{
		Gage += GageRate;
	}
}

void APaladin::ESCDown()
{
	bESCDown = true;

	if(PaladinPlayerController)
	{
		// UnPossessed(); ��Ʈ�ѷ��� ���� ������ Resume�� �ȵȴ�. -> SetPause�� ����ϸ� World��ü�� ���ߴ� ���� ��Ʈ�ѷ� ������ �� �ʿ䰡 ����.
		// ���� ���ǿ����� CanMove()��� �Լ��� ����� ��Ʈ���� �����ϴµ� �ʹ� ��ȿ�������� ���δ�.
		PaladinPlayerController->TogglePauseMenu();
	}
}

//bool APaladin::CanMove(float Value) ��Ʈ�� �� ������ �� ������ ó���ϴ� ����� �ƹ��� �����ص� �ƴϴ�.
//{
//	if(PaladinPlayerController)
//	{
//		return Value != 0.0f && !bAttacking() Attacking���� ���� ���� ���̶�� Ư�� ��Ȳ���� �����ϴ� ���̱� ������ ������ �ȴ�.
//			&& MovementStatus != EMovementStatus::EMS_Dead ���� ��Ȳ�̶�� ��Ʈ�ѷ��� UnPossess�ϰ� ������ �� �ٽ� Possess�ϴ� ����� ���� �ʳ�?
//			&& !PaladinPlayerController->bPauseMenuVisible;  �̷������� ��� �þ�°� �ʹ� ��ȿ�����̶� �����ȴ�. 
//	}		�����ؾ��� ��ư�� ���������� ������ ��ȿ������ ���̴�. ������ �Լ��� �߰��ؾ��ϰ� ���󿡼��� Jump�� �߰��ϴ� ���� �����ߴ�.
//			��ۿ��� AGameModeBase::SetPause�Լ��� EnableInput, DisableInput�Լ��� ����ϴ� �Ϳ� ���� �̾߱� �ϰ� �ִ�.
//	return false;
//}

void APaladin::ESCUp()
{
	bESCDown = false;
}

void APaladin::SetEquipWeapon(AWeapon* WeaponToSet)
{
	if(EquipWeapon)
	{
		EquipWeapon->Destroy();
	}

	EquipWeapon = WeaponToSet;
}

void APaladin::Attack()
{	
	if(!bAttacking)
	{
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance && CombatMontage)
		{
			int32 Section = FMath::RandRange(0, 1);
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;
			default:
					;
			}
		}
	}
}

void APaladin::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if(bLMBDown)
	{
		Attack();
	}
}

void APaladin::PlaySwingSound()
{
	if(EquipWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquipWeapon->SwingSound);
	}
}

void APaladin::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void APaladin::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if(OverlappingActors.Num() == 0)
	{
		if(PaladinPlayerController)
		{
			PaladinPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	if(ClosestEnemy)
	{
		FVector Location = GetActorLocation();

		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if(DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}

		if(PaladinPlayerController)
		{
			PaladinPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void APaladin::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();

	if(World)
	{
		FString CurrentLevel = World->GetMapName();

		if(*CurrentLevel != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void APaladin::SaveGame()
{
	USTRSaveGame* SaveGameInstance = Cast<USTRSaveGame>(UGameplayStatics::CreateSaveGameObject(USTRSaveGame::StaticClass()));

	// �ʹ� ��ȿ������ �ڵ� ����.
	// ���� CharacterStats�� ���߿� ����� �׷��������� ���Ŀ� ĳ���� ������ �����ϴ� ������� �����ؾ߰ڴ�.
	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	SaveGameInstance->CharacterStats.LevelName = MapName;

	if(EquipWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquipWeapon->Name;
	}

	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void APaladin::LoadGame(bool SetPosition)
{
	USTRSaveGame* LoadGameInstance = Cast<USTRSaveGame>(UGameplayStatics::CreateSaveGameObject(USTRSaveGame::StaticClass()));

	LoadGameInstance = Cast<USTRSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	if(WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if(Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			if(Weapons->WeaponMap.Contains(WeaponName))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);

				WeaponToEquip->Equip(this);
			}
		}
	}

	if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
	{
		FName LevelName(*LoadGameInstance->CharacterStats.LevelName);

		SwitchLevel(LevelName);
	}

	if(SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}
}

void APaladin::LoadGameNoSwitch() // Map�� �̵��� �� ������ �������ֱ� ���� �Լ���.
{
	USTRSaveGame* LoadGameInstance = Cast<USTRSaveGame>(UGameplayStatics::CreateSaveGameObject(USTRSaveGame::StaticClass()));

	LoadGameInstance = Cast<USTRSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			if (Weapons->WeaponMap.Contains(WeaponName))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);

				WeaponToEquip->Equip(this);
			}
		}
	}
}

void APaladin::SetChargeBegin()
{
	bChargeAttack = true;
}

void APaladin::SetChargeEnd()
{
	Gage = 0;
	bChargeAttack = false;
}
