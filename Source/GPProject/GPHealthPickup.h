// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GPPickup.h"
#include "GPHealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPHealthPickup : public AGPPickup
{
	GENERATED_BODY()
	
    AGPHealthPickup(const FObjectInitializer& ObjectInitializer);
	
public:
    /** called when something enters the sphere component */
    UFUNCTION()
    void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    /* Spawns health box in a random location */
    UFUNCTION()
    void SpawnHealth(class AActor* HealthOwner);
};
