// Fill out your copyright notice in the Description page of Project Settings.


#include "PaladinAnimInstance.h"

#include "GameFramework/PawnMovementComponent.h"

void UPaladinAnimInstance::NativeInitializeAnimation()
{
	if(Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}
}

void UPaladinAnimInstance::UpdateAnimationProperties()
{
	if(Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if(Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
}
