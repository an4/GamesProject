// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPFlagPickup.h"

AGPFlagPickup::AGPFlagPickup(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("StaticMesh'/Game/ExampleContent/Input_Examples/Meshes/SM_Toy_Tree.SM_Toy_Tree'"));
    
    // static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("StaticMesh'/Game/Meshes/GP_Cube.GP_Cube'"));

    static ConstructorHelpers::FObjectFinder<UMaterial> Material_Green(TEXT("Material'/Game/ExampleContent/Input_Examples/Materials/M_Toy_Tree.M_Toy_Tree'"));

    if (StaticMesh.Object) {
        PickupMesh->SetStaticMesh(StaticMesh.Object);
    }

    PickupMesh->SetMaterial(0, Material_Green.Object);

    BaseCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGPFlagPickup::OnOverlapBegin);
}

void AGPFlagPickup::OnPickedUp() {
    Super::OnPickedUp();
}

void AGPFlagPickup::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    GetWorld()->DestroyActor(this, true);
    SpawnFlag(OtherActor);
}

void AGPFlagPickup::SpawnFlag(class AActor* FlagOwner)
{
    UWorld* const World = GetWorld();

    if (World)
    {
        FActorSpawnParameters SpawnParams = FActorSpawnParameters();

        SpawnParams.Owner = FlagOwner;
        SpawnParams.Instigator = NULL;

        FRotator rotation = FRotator(0.f, 0.f, 0.f);
        FVector location = FMath::RandPointInBox(FBox(FVector(-2500., -2500., 21.), FVector(2500., 2500., 21.)));

        AGPFlagPickup* flag = World->SpawnActor<AGPFlagPickup>(AGPFlagPickup::StaticClass(), location, rotation, SpawnParams);

        if (flag == NULL)
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Flag is null"));
            }
        }
    }
}