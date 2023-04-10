// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class SUMTEMPLERPG_API APickup : public AItem
{
	GENERATED_BODY()

public:
	APickup();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	UStaticMeshComponent* Mesh;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPickupBP(class APaladin* Target);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Mesh")
	bool bSineMove;

private:
	float RunningTime;

protected:
	virtual void Tick(float DeltaSeconds) override;
};
