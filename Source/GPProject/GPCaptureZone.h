// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GPCaptureZone.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPCaptureZone : public AActor
{
	GENERATED_BODY()

	public:
		AGPCaptureZone(const FObjectInitializer& ObjectInitializer);
		
		/** called when something enters the sphere component */
		UFUNCTION()
		void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	/** True when the pickup is able to be picked up, false if something deactivates the pickup. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Area)
		bool bIsActive;

	/** Simple collision primitive to use as the root component. */
		UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Area)
		USphereComponent* BaseCollisionComponent;

	/** StaticMeshComponent to represent the pickup in the level. */
		UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Area)
		UStaticMeshComponent* PickupMesh;
	
};
