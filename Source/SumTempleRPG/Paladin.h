// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Paladin.generated.h"

UCLASS()
class SUMTEMPLERPG_API APaladin : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APaladin();

	// 톰 루먼 강사는 protected에 선언하는 것이 권고사항이라 들었지만 public에 할당하고 있다.
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

	void IncrementCoin(int32 Amount);

	void Die();

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

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);
};
