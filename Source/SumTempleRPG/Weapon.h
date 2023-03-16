// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 *  Item class�� ��� �޾Ҵµ� ���� �̰� ��ӹ޾ƾ� �ϳ� �ʹ�.
 *	�� class ������ Item class�� OnOverlapBegin �Լ��� �ִ� Destroy() �Լ��� �����µ� �̰� ���� ���� ��Ȳ�� �ƴұ� �ʹ�.
 */
UCLASS()
class SUMTEMPLERPG_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	USkeletalMeshComponent* SkeletalMesh;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void Equip(class APaladin* Character);
};
