// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPAmmoPickup.h"
#include "GPCharacter.h"

AGPAmmoPickup::AGPAmmoPickup(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("StaticMesh'/Game/ExampleContent/healthBox.healthBox'"));
    static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/Red.Red'"));

    if (StaticMesh.Object) {
        PickupMesh->SetStaticMesh(StaticMesh.Object);
    }

    PickupMesh->SetMaterial(0, Material.Object);

    BaseCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGPAmmoPickup::OnOverlapBegin);

    Value = FMath::RandRange(50, 100);
}

void AGPAmmoPickup::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AGPCharacter* const currentActor = Cast<AGPCharacter>(OtherActor);

    GetWorld()->DestroyActor(this, true);

    if (Role == ROLE_Authority) {
        SpawnAmmo(OtherActor);
    }

    if (currentActor) {
        currentActor->OnAmmoPickUp(this->getValue());
    }
}

void AGPAmmoPickup::SpawnAmmo(class AActor* AmmoOwner)
{
    UWorld* const World = GetWorld();

    if (World)
    {
        FActorSpawnParameters SpawnParams = FActorSpawnParameters();

        SpawnParams.Owner = AmmoOwner;
        SpawnParams.Instigator = NULL;

        FRotator rotation = FRotator(0.f, 0.f, 0.f);
        FVector location = FMath::RandPointInBox(FBox(FVector(-2800., -4300., 50.), FVector(2800., 4300., 50.)));

        AGPAmmoPickup* ammo = World->SpawnActor<AGPAmmoPickup>(AGPAmmoPickup::StaticClass(), location, rotation, SpawnParams);
    }
}

int32 AGPAmmoPickup::getValue()
{
    return Value;
}
