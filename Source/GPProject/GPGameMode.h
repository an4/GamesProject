// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GPBuilding.h"
#include "GPFlagPickup.h"
#include "GameFramework/GameMode.h"
#include "GPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPGameMode : public AGameMode
{
	GENERATED_BODY()

	double tickCount;

	bool IsClear(FVector2D centre, FRotator rotation, FVector scale);

	virtual void StartPlay() override;
	AGPGameMode(const class FObjectInitializer& ObjectInitializer);

	public:
		/** Building class to spawn */
		UPROPERTY(EditDefaultsOnly, Category = Building)
		TSubclassOf<class AGPBuilding> BuildingClass;

		void SpawnBuilding(FVector2D a, FVector2D b);

		UFUNCTION()
		void SpawnBuilding(FVector centre, FRotator rotation, FVector scale);

		void Tick(float DeltaSeconds) override;

		/** Flag Pickup class to spawn */
        UPROPERTY(EditDefaultsOnly, Category = Pickup)
        TSubclassOf<class AGPFlagPickup> FlagPickupClass;

        UFUNCTION()
        void SpawnFlag();

		UFUNCTION()
		void ResetBuildings();
};

