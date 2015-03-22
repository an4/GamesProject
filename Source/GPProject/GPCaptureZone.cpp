// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPCaptureZone.h"
#include "GPCharacter.h"
#include "GPPlayerState.h"

AGPCaptureZone::AGPCaptureZone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// The pickup is valid when it is created.
	bIsActive = true;

	// Create the root SphereComponent to handle the pickup's collision
	BaseCollisionComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("BaseSphereComponent"));

	// Set the SphereComponent as the root component.
	RootComponent = BaseCollisionComponent;

	// Create the StaticMeshComponent.
	//PickupMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PickupMesh"));

	// Attach the StaticMeshComponent to the RootComponent.
	//PickupMesh->AttachTo(RootComponent);

	//PickupMesh->SetSimulatePhysics(true);

	bReplicates = true;

	BaseCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGPCaptureZone::OnOverlapBegin);
}

void AGPCaptureZone::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check we have an actor
	AGPCharacter* const currentActor = Cast<AGPCharacter>(OtherActor);
	if (currentActor)
	{
		// Check that actor has a state
		AGPPlayerState* PState = (AGPPlayerState*)currentActor->PlayerState;
		if (PState)
		{
			// Check we have the flag
			if (PState->GetHasFlag()) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("We have a flag!"));
				// Tell actor to cap it!
				currentActor->OnFlagCapture();
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No flag for you :("));
			}
		}
	}
}

