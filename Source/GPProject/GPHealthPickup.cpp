// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPHealthPickup.h"
#include "GPCharacter.h"

AGPHealthPickup::AGPHealthPickup(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("StaticMesh'/Game/ExampleContent/healthBox.healthBox'"));

    static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/ExampleContent/TexHealthBox.TexHealthBox'"));

    if (StaticMesh.Object) {
        PickupMesh->SetStaticMesh(StaticMesh.Object);
    }

    PickupMesh->SetMaterial(0, Material.Object);

    BaseCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGPHealthPickup::OnOverlapBegin);

}

void AGPHealthPickup::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AGPCharacter* const currentActor = Cast<AGPCharacter>(OtherActor);

    if (currentActor) {
        if (currentActor->getHealth() == 100.0f) {
            return;
        }
    }

    GetWorld()->DestroyActor(this, true);

    if (Role == ROLE_Authority) {
        SpawnHealth(OtherActor);
    }

    if (currentActor) {
        currentActor->OnHealthPickUp();
    }
}

void AGPHealthPickup::SpawnHealth(class AActor* HealthOwner)
{
    UWorld* const World = GetWorld();

    if (World)
    {
        FActorSpawnParameters SpawnParams = FActorSpawnParameters();

        SpawnParams.Owner = HealthOwner;
        SpawnParams.Instigator = NULL;

        FRotator rotation = FRotator(0.f, 0.f, 0.f);
        FVector location = FMath::RandPointInBox(FBox(FVector(-2800., -4300., 50.), FVector(2800., 4300., 50.)));

        AGPHealthPickup* health = World->SpawnActor<AGPHealthPickup>(AGPHealthPickup::StaticClass(), location, rotation, SpawnParams);
    }
}