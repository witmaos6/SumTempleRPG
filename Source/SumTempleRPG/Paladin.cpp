// Fill out your copyright notice in the Description page of Project Settings.


#include "Paladin.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"

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

void APaladin::DecrementHealth(float Amount)
{
	Health -= Amount;
	if(Health - Amount <= 0.f)
	{
		Die();
	}
}

void APaladin::IncrementCoin(int32 Amount)
{
	Coins += Amount;
}

void APaladin::Die()
{

}

// Called when the game starts or when spawned
void APaladin::BeginPlay()
{
	Super::BeginPlay();

	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;
}

// Called every frame
void APaladin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
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
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
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
}

// Called to bind functionality to input
void APaladin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); // if(PlayerInputComponent) 와 같은 기능이다. 할당이 안돼 있다면 함수를 종료시킨다.

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APaladin::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APaladin::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &APaladin::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &APaladin::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &APaladin::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APaladin::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APaladin::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APaladin::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &APaladin::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APaladin::LookUpAtRate);
}

void APaladin::MoveForward(float Value)
{
	if(Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APaladin::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
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
}

void APaladin::LMBUp()
{
	bLMBDown = false;
}

void APaladin::SetEquipWeapon(AWeapon* WeaponToSet)
{
	if(EquipWeapon)
	{
		EquipWeapon->Destroy();
	}

	EquipWeapon = WeaponToSet;
}

