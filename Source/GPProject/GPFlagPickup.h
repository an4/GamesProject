// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GPPickup.h"
#include "GPFlagPickup.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPFlagPickup : public AGPPickup
{
	GENERATED_BODY()
	
    AGPFlagPickup(const FObjectInitializer& ObjectInitializer);

public:

    void OnPickedUp() override;

    /** called when something enters the sphere component */
    UFUNCTION()
    void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
