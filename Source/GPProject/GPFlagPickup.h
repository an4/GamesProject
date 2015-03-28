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
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* GreenMaterial;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* RedMaterial;

	UFUNCTION()
	void Init(int8 Team);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetMaterial(int8 Team);
	bool ServerSetMaterial_Validate(int8 Team);
	void ServerSetMaterial_Implementation(int8 Team);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastSetMaterial(int8 Team);
	void BroadcastSetMaterial_Implementation(int8 Team);

	UFUNCTION()
	void ClientOnlySetMaterial();

    /** called when something enters the sphere component */
    UFUNCTION()
    void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UPROPERTY(EditDefaultsOnly, Category = Sounds)
    USoundCue* PickUpSound;

	UPROPERTY(Replicated)
	int8 flagTeam;
};
