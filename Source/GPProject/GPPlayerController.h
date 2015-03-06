// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/PlayerController.h"
#include "GPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetupInputComponent() override;

	//handles moving forward/backward
    UFUNCTION()
    void MoveForward(float Val);
    //handles strafing
    UFUNCTION()
    void MoveRight(float Val);

	//handles turning
	UFUNCTION()
	void AddControllerYawInput(float Value);
	//handles up/down look
	UFUNCTION()
	void AddControllerPitchInput(float Value);

    //sets jump flag when key is pressed
    UFUNCTION()
    void OnStartJump();
    //clears jump flag when key is released
    UFUNCTION()
	void OnStopJump();

	//handles firing
	void OnFire();

	//handles bomb launching
	void OnBombLaunch();

	//handles bomb detonation
	void OnBombDetonate();
	//UFUNCTION(Reliable, Server, WithValidation)
	//void ServerOnFire();

	//// Following code taken from the replication wiki.

	//bool bSomeBool;

	//void SetSomeBool(bool bNewSomeBool);

	//UFUNCTION(reliable, server, WithValidation)
	//void ServerSetSomeBool(bool bNewSomeBool);
	
};
