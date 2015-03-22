// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPCharacter.h"
#include "GPFlagPickup.h"
#include "GPGameMode.h"

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

    static ConstructorHelpers::FObjectFinder<USoundCue> PickUpSoundCueLoader(TEXT("SoundCue'/Game/Audio/PickUp_Cue.PickUp_Cue'"));
    PickUpSound = PickUpSoundCueLoader.Object;
}

void AGPFlagPickup::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check we have an actor that can pick up a flag before doing so
	AGPCharacter* const currentActor = Cast<AGPCharacter>(OtherActor);
	if (currentActor)
	{
		if (currentActor->CanPickupFlag()) {
			this->PlaySoundOnActor(PickUpSound, 0.5f, 0.5f);

			GetWorld()->DestroyActor(this, true);

			// Moved flag spawning into the actor so that we can call the server to do it without needing flags to have an owner
			currentActor->OnFlagPickup();
		}
	}
    /*this->PlaySoundOnActor(PickUpSound, 0.5f, 0.5f);

    GetWorld()->DestroyActor(this, true);

    if (Role == ROLE_Authority) {
        SpawnFlag(OtherActor);
    }

    AGPCharacter* const currentActor = Cast<AGPCharacter>(OtherActor);
    if (currentActor) {
        currentActor->OnFlagPickup();
    }*/
}