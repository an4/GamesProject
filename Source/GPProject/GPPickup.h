// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GPPickup.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPPickup : public AActor
{
	GENERATED_BODY()

public:
    AGPPickup(const FObjectInitializer& ObjectInitializer);


    /** True when the pickup is able to be picked up, false if something deactivates the pickup. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
    bool bIsActive;

    /** Simple collision primitive to use as the root component. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    USphereComponent* BaseCollisionComponent;

    /** StaticMeshComponent to represent the pickup in the level. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    UStaticMeshComponent* PickupMesh;

    /** Function to call when the Pickup is collected. */
    virtual void OnPickedUp();	
};
