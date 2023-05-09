// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill.h"

#include "DrawDebugHelpers.h"
#include "Enemy.h"
#include "Paladin.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ASkill::ASkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SkillDamage = 50.f;
	CollisionRadius = 350.f;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	CollisionSphere->InitSphereRadius(CollisionRadius);
}

void ASkill::BeginPlay()
{
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASkill::CollisionSphereOnOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ASkill::CollisionSphereOnOverlapEnd);

	FVector SpawnLocation = GetActorLocation();
	SpawnEffect(SpawnLocation); // 스폰 위치를 Paladin 을 기준으로 앞에 하고 싶은데 도저히 답이 안나온다.
}

void ASkill::SpawnEffect(const FVector& Location)
{
	if(SkillParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SkillParticle, Location, FRotator(0.f), true);
	}
	if(SkillSound)
	{
		UGameplayStatics::PlaySound2D(this, SkillSound);
	}
}

void ASkill::CollisionSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	if(Enemy)
	{
		UGameplayStatics::ApplyDamage(Enemy, SkillDamage, GetInstigatorController(), this, DamageTypeClass);
	}

	Destroy(); // Temp
}

void ASkill::CollisionSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 액터의 소멸은 경우의 수가 다양하기 때문에 여기서 설정할 수 없다고 생각된다.
}

