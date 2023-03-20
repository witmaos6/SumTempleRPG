// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Equipped UMETA(DisplayName = "Equiped"),

	EWS_MAX UMETA(DisplayName = "DefaultMax")
};
/**
 *  
 */
UCLASS()
class SUMTEMPLERPG_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	EWeaponState WeaponState;

	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	USoundCue* OnEquipSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	USkeletalMeshComponent* SkeletalMesh;
	// Character socket에 넣기 위해서는 static mesh가 아닌 Skeletal Mesh Component를 사용해야 한다.
	// 지금 Item에 UStaticMeshComponent가 있고 Weapon에는 할당하지 않은 상태이기 때문에 문제가 발생하고 있다.
	// class 설계 실패로 봐야하는 상황이 아닐까 싶다.

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void Equip(class APaladin* Character);
};
