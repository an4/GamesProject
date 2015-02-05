// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPPlayerController.h"
#include "GPCharacter.h"

// The following code is taken from the replication wiki. Details how to update a boolean property on the server from a client.

void AGPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// set up gameplay key bindings
	InputComponent->BindAxis("MoveForward", this, &AGPPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGPPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AGPPlayerController::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &AGPPlayerController::AddControllerPitchInput);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AGPPlayerController::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &AGPPlayerController::OnStopJump);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AGPPlayerController::OnFire);
}

void AGPPlayerController::MoveForward(float Value)
{
	// TODO: Health test - forward disabled when health gone
    if (GetCharacter() != NULL && (Value != 0.0f) /*&& (Health > 0.0f)*/)
    {
        // find out which way is forward
        FRotator Rotation = GetControlRotation();
        // Limit pitch when walking or falling
		if (GetCharacter()->GetCharacterMovement()->IsMovingOnGround() || GetCharacter()->GetCharacterMovement()->IsFalling())
        {
            Rotation.Pitch = 0.0f;
        }
        // add movement in that direction
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        GetCharacter()->AddMovementInput(Direction, Value);
    }
}

void AGPPlayerController::MoveRight(float Value)
{
    if ((GetCharacter() != NULL) && (Value != 0.0f))
    {
        // find out which way is right
        const FRotator Rotation = GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        // add movement in that direction
        GetCharacter()->AddMovementInput(Direction, Value);
    }
}

void AGPPlayerController::AddControllerYawInput(float Value)
{
	if ((GetCharacter() != NULL) && (Value != 0.0f))
	{
		GetCharacter()->AddControllerYawInput(Value);
	}
}

void AGPPlayerController::AddControllerPitchInput(float Value)
{
	if ((GetCharacter() != NULL) && (Value != 0.0f))
	{
		GetCharacter()->AddControllerPitchInput(Value);
	}
}

void AGPPlayerController::OnStartJump()
{
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnStartJump();
	}
}

void AGPPlayerController::OnStopJump()
{
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnStopJump();
	}
}

void AGPPlayerController::OnFire()
{
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnFire();
	}
}

//bool AGPPlayerController::ServerOnFire_Validate()
//{
//	// We should check game state down the road to make sure this is valid.
//	return true;
//}
//
//void AGPPlayerController::ServerOnFire_Implementation()
//{
//	if (GetCharacter() != NULL)
//	{
//		Cast<AGPCharacter>(GetCharacter())->OnFire();
//	}
//}

//void AGPPlayerController::SetSomeBool(bool bNewSomeBool)
//{
//	// Change the value of the bSomeBool property
//	bSomeBool = bNewSomeBool;
//
//	// If this next check succeeds, we are *not* the authority, meaning we are a network client.
//	// In this case we also want to call the server function to tell it to change the bSomeBool property as well.
//	if (Role < ROLE_Authority)
//	{
//		ServerSetSomeBool(bNewSomeBool);
//	}
//}
//
//bool AGPPlayerController::ServerSetSomeBool_Validate(bool bNewSomeBool)
//{
//	return true;
//}
//
//void AGPPlayerController::ServerSetSomeBool_Implementation(bool bNewSomeBool)
//{
//	// This function is only called on the server (where Role == ROLE_Authority), called over the network by clients.
//	// We need to call SetSomeBool() to actually change the value of the bool now!
//	// Inside that function, Role == ROLE_Authority, so it won't try to call ServerSetSomeBool() again.
//	SetSomeBool(bNewSomeBool);
//}