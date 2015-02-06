// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GPBuilding.h"
#include "GameFramework/GameMode.h"
#include "GPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPGameMode : public AGameMode
{
    GENERATED_BODY()

	virtual void StartPlay() override;
	AGPGameMode(const class FObjectInitializer& ObjectInitializer);

	AGPBuilding building;
	FActorSpawnParameters SpawnParams;

	public:
		/** Building class to spawn */
		UPROPERTY(EditDefaultsOnly, Category = Building)
		TSubclassOf<class AGPBuilding> BuildingClass;



};

