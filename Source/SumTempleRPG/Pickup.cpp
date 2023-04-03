// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "Paladin.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APickup::APickup()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(OtherActor)
	{
		APaladin* Paladin = Cast<APaladin>(OtherActor);
		if(Paladin)
		{
			OnPickupBP(Paladin);
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}

			Destroy();
		}
	}
}

void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
