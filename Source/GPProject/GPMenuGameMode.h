// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "GPMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPMenuGameMode : public AGameMode
{
	GENERATED_BODY()
	
	public:
	UFUNCTION(BlueprintCallable, Category = "Projection")
	void SetProjectingPlayerIPAddress(APlayerController* InController);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetProjectingPlayerIPAddress(const FString& IP);

	bool ServerSetProjectingPlayerIPAddress_Validate(const FString& IP);
	void ServerSetProjectingPlayerIPAddress_Implementation(const FString& IP);
	
};
