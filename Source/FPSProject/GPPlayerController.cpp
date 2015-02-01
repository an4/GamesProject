// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSProject.h"
#include "GPPlayerController.h"

// The following code is taken from the replication wiki. Details how to update a boolean property on the server from a client.

void AGPPlayerController::SetSomeBool(bool bNewSomeBool)
{
	// Change the value of the bSomeBool property
	bSomeBool = bNewSomeBool;

	// If this next check succeeds, we are *not* the authority, meaning we are a network client.
	// In this case we also want to call the server function to tell it to change the bSomeBool property as well.
	if (Role < ROLE_Authority)
	{
		ServerSetSomeBool(bNewSomeBool);
	}
}

bool AGPPlayerController::ServerSetSomeBool_Validate(bool bNewSomeBool)
{
	return true;
}

void AGPPlayerController::ServerSetSomeBool_Implementation(bool bNewSomeBool)
{
	// This function is only called on the server (where Role == ROLE_Authority), called over the network by clients.
	// We need to call SetSomeBool() to actually change the value of the bool now!
	// Inside that function, Role == ROLE_Authority, so it won't try to call ServerSetSomeBool() again.
	SetSomeBool(bNewSomeBool);
}