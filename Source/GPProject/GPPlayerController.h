// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/PlayerController.h"
#include "GPCharacter.h"
#include "GPServerPawn.h"
#include "GPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPPlayerController : public APlayerController
{
    GENERATED_BODY()

    bool InTeam = false;
public:

    AGPPlayerController(const FObjectInitializer& ObjectInitializer);

    bool GetPlayerPawnClass();

	virtual void SetupInputComponent() override;
    bool IsServerPlayer = false;

    UFUNCTION(BlueprintCallable, Category = "Team")
	void JoinTeam0();

    UFUNCTION(BlueprintCallable, Category = "Team")
	void JoinTeam1();

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

	// Handles rescan requesting
	void OnRequestRescan();

	//UFUNCTION(Reliable, Server, WithValidation)
	//void ServerOnFire();

	//// Following code taken from the replication wiki.

	//bool bSomeBool;

	//void SetSomeBool(bool bNewSomeBool);

	//UFUNCTION(reliable, server, WithValidation)
	//void ServerSetSomeBool(bool bNewSomeBool);
    UPROPERTY(Replicated, BluePrintReadWrite, Category = "Projection")
    bool isProjecting = true;

    protected:
    /* Return The Correct Pawn Class Client-Side */
    UFUNCTION(Reliable, Client)
    void DeterminePawnClass();
    virtual void DeterminePawnClass_Implementation();

    /* Use BeginPlay to start the functionality */
    virtual void BeginPlay() override;

    /* Set Pawn Class On Server For This Controller */
    UFUNCTION(Reliable, Server, WithValidation)
    virtual void ServerSetPawn(bool InIsProjecting);
    virtual void ServerSetPawn_Implementation(bool InIsProjecting);
    virtual bool ServerSetPawn_Validate(bool InIsProjecting);
	
};
