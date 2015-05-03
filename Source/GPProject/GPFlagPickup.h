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

	void Tick(float DeltaSeconds) override;

	UPROPERTY(Replicated)
	float timeAlive;

	UPROPERTY(Replicated)
	bool wasDropped;

	UPROPERTY(Replicated)
	float timeToLive = 10.f;

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* GreenMaterial;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* RedMaterial;

	UFUNCTION()
	void Init(int8 Team, bool isNew);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetMaterial(int8 Team);
	bool ServerSetMaterial_Validate(int8 Team);
	void ServerSetMaterial_Implementation(int8 Team);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastSetMaterial(int8 Team);
	void BroadcastSetMaterial_Implementation(int8 Team);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetLight(int8 Team, float val);
	bool ServerSetLight_Validate(int8 Team, float val);
	void ServerSetLight_Implementation(int8 Team, float val);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastSetLight(int8 Team, float val);
	void BroadcastSetLight_Implementation(int8 Team, float val);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMoveToSpawn();
	bool ServerMoveToSpawn_Validate();
	void ServerMoveToSpawn_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastMoveToSpawn();
	void BroadcastMoveToSpawn_Implementation();

	UFUNCTION()
	void ClientOnlySetMaterial();

    /** called when something enters the sphere component */
    UFUNCTION()
    void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(Replicated)
	int8 flagTeam = -1;

	UFUNCTION(BlueprintCallable, Category = "FlagDrop")
	float GetTimeAlive();

	UFUNCTION(BlueprintCallable, Category = "FlagDrop")
	bool GetWasDropped();

	UFUNCTION(BlueprintCallable, Category = "FlagDrop")
	float GetTimeToLive();

	UPROPERTY(Replicated)
	UTextRenderComponent * textRender;
};
