// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUMTEMPLERPG_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float ChargeCoolDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float CastingCoolDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float ComboCoolDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float ChargeCoolState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float CastingCoolState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	float ComboCoolState;

	void ChargeCoolInit();

	void CastingCoolInit();

	void ComboCoolInit();

	void ApplyCoolDown(float& CoolState, float CoolDown);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
