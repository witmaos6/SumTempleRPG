// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Skill.generated.h"

UCLASS()
class SUMTEMPLERPG_API ASkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkill();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float CollisionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	float SkillDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SkillEffect")
	UParticleSystem* SkillParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SkillEffect")
	class USoundCue* SkillSound;
protected:
	virtual void BeginPlay() override;

public:
	void SpawnEffect(const FVector& Location);

	UFUNCTION()
	virtual void CollisionSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void CollisionSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
