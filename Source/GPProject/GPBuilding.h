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

	AGPBuilding(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
    
public:

    UPROPERTY()
    class UBoxComponent* DummyRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = StaticMeshComponents)
	UStaticMeshComponent* BuildingMesh;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Scale)
	FVector Scale;

	UFUNCTION()
	void OnRep_Scale();

	UFUNCTION()
	void SetScale(FVector AbsoluteScale);

};
