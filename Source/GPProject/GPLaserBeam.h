// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GPLaserBeam.generated.h"

UCLASS()
class GPPROJECT_API AGPLaserBeam : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPLaserBeam(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY()
	class USceneComponent* DummyRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = StaticMeshComponents)
	UStaticMeshComponent* LaserMesh;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Scale)
	FVector Scale;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Angle)
	FRotator Angle;

	UFUNCTION()
	void OnRep_Scale();

	UFUNCTION()
	void OnRep_Angle();

	UFUNCTION()
	void SetLengthAndPitch(float Length, float Pitch);
};
