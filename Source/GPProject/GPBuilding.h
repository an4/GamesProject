// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GPBuilding.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPBuilding : public AActor
{
	GENERATED_BODY()


	public:
		//UStaticMeshComponent* BuildingMesh;
		AGPBuilding(const FObjectInitializer& ObjectInitializer);

	
};
