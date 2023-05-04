// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Enemy.h"
#include "Paladin.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(RootComponent);

	bWeaponParticle = false;

	WeaponState = EWeaponState::EWS_Pickup;

	Damage = 25.f;
	ChargeDamage = 0.f;
	DamageRadial = 200.f;
	CastingDamage = 50.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(WeaponState == EWeaponState::EWS_Pickup && OtherActor)
	{
		APaladin* Paladin = Cast<APaladin>(OtherActor);

		if(Paladin)
		{
			Paladin->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		APaladin* Paladin = Cast<APaladin>(OtherActor);

		if (Paladin)
		{
			Paladin->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(APaladin* Character)
{
	if(Character)
	{
		OwnerPaladin = Character;
		SetInstigator(Character->GetController());

		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");

		if(RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Character->GetMesh());
			bRotate = false;
			
			Character->SetEquipWeapon(this);
			Character->SetActiveOverlappingItem(nullptr);

			if (OnEquipSound)
			{
				UGameplayStatics::PlaySound2D(this, OnEquipSound);
			}
			if(!bWeaponParticle)
			{
				IdleParticlesComponent->Deactivate();
			}
		}
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		if(Enemy)
		{
			if(Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				if(WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, GetActorLocation(), FRotator(0.f), false);
				}
			}
			if(Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if(DamageTypeClass)
			{
				if(OwnerPaladin && OwnerPaladin->bChargeAttack)
				{
					UGameplayStatics::ApplyDamage(Enemy, Damage + ChargeDamage, WeaponInstigator, this, DamageTypeClass);
				}
				else
				{
					UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
				}
			}
		}
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::RadialAttack()
{
	TArray<AActor*> IgnoreActor;
	IgnoreActor.Add(this);
	IgnoreActor.Add(OwnerPaladin);

	const FVector CastingAttackLocation = GetActorLocation();

	UGameplayStatics::ApplyRadialDamage(this, CastingDamage, CastingAttackLocation, DamageRadial, DamageTypeClass, IgnoreActor, this, WeaponInstigator, true);

	if (CastingParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CastingParticle, CastingAttackLocation, GetActorRotation(), false);
	}
	if (CastingSound)
	{
		UGameplayStatics::PlaySound2D(this, CastingSound);
	}
}
