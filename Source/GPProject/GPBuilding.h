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
    class USceneComponent* DummyRoot;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Scale)
	class UMaterial* texLego;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Scale)
	class UMaterial* texBook;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Scale)
	class UMaterial* texTruck;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = StaticMeshComponents)
	UStaticMeshComponent* Building;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = StaticMeshComponents)
	UStaticMesh* BuildingMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = StaticMeshComponents)
	UStaticMesh* BuildingMesh2;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Scale)
	FVector Scale;

	UFUNCTION()
	void OnRep_Scale();

	UFUNCTION()
	void SetScale(FVector AbsoluteScale);
};
