// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent.h"

#include "Paladin.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ChargeCoolState = 0.f;
	CastingCoolState = 0.f;
	ComboCoolState = 0.f;

	ChargeCoolDown = 15.f;
	CastingCoolDown = 10.f;
	ComboCoolDown = 5.f;
}


void USkillComponent::ChargeCoolInit()
{
	if(ChargeCoolState > 0.f)
	{
		ChargeCoolState -= 1.f;
	}
	else
	{
		AActor* MyActor = GetOwner();

		APaladin* Paladin = Cast<APaladin>(MyActor);

		if(Paladin)
		{
			Paladin->GetWorldTimerManager().ClearTimer(Paladin->ChargeCoolTimer);
		}
	}
}

void USkillComponent::CastingCoolInit()
{
	if(CastingCoolState > 0.f)
	{
		CastingCoolState -= 1.f;
	}
	else
	{
		AActor* MyActor = GetOwner();

		APaladin* Paladin = Cast<APaladin>(MyActor);

		if (Paladin)
		{
			Paladin->GetWorldTimerManager().ClearTimer(Paladin->CastingCoolTimer);
		}
	}
}

void USkillComponent::ComboCoolInit()
{
	if (ComboCoolState > 0.f)
	{
		ComboCoolState -= 1.f;
	}
	else
	{
		AActor* MyActor = GetOwner();

		APaladin* Paladin = Cast<APaladin>(MyActor);

		if (Paladin)
		{
			Paladin->GetWorldTimerManager().ClearTimer(Paladin->ComboCoolTimer);
		}
	}
}

void USkillComponent::ApplyCoolDown(float& CoolState, float CoolDown)
{
	CoolState = CoolDown;
}

// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

