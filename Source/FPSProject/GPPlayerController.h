// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "GPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API AGPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	// Following code taken from the replication wiki.

	bool bSomeBool;

	void SetSomeBool(bool bNewSomeBool);

	UFUNCTION(reliable, server, WithValidation)
	void ServerSetSomeBool(bool bNewSomeBool);
	
};
