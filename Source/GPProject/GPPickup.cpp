// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPPickup.h"

AGPPickup::AGPPickup(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // The pickup is valid when it is created.
    bIsActive = true;

    // Create the root SphereComponent to handle the pickup's collision
    BaseCollisionComponent =  ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("BaseSphereComponent"));

    // Set the SphereComponent as the root component.
    RootComponent = BaseCollisionComponent;

    // Create the StaticMeshComponent.
    PickupMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PickupMesh"));

    // Attach the StaticMeshComponent to the RootComponent.
    PickupMesh->AttachTo(RootComponent);

    //PickupMesh->SetSimulatePhysics(true);

    bReplicates = true;

    static ConstructorHelpers::FObjectFinder<USoundCue> PickUpSoundCueLoader(TEXT("SoundCue'/Game/Audio/PickUp_Cue.PickUp_Cue'"));
    PickUpSound = PickUpSoundCueLoader.Object;
}

void AGPPickup::playSound()
{
    this->PlaySoundOnActor(PickUpSound, 0.5f, 0.5f);
}