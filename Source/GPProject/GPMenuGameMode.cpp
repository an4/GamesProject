// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPMenuGameMode.h"
#include "GPGameInstance.h"

void AGPMenuGameMode::SetProjectingPlayerIPAddress(APlayerController* InController)
{
	FString address = InController->GetPlayerNetworkAddress();
	FString temp = address + FString("is what should be going in...");
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, *temp);

	ServerSetProjectingPlayerIPAddress(address);
}

bool AGPMenuGameMode::ServerSetProjectingPlayerIPAddress_Validate(const FString& IP)
{
	return true;
}

void AGPMenuGameMode::ServerSetProjectingPlayerIPAddress_Implementation(const FString& IP)
{
	if (Role == ROLE_Authority)
	{
		FString temp = FString("Setting this IP: ") + IP;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, *temp);
		UGPGameInstance* Instance = Cast<UGPGameInstance>(GetGameInstance());
		Instance->ProjectingIP = IP;
	}
}