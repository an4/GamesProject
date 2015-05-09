// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPPlayerController.h"
#include "GPGameState.h"
#include "UnrealNetwork.h"
#include "GPGameMode.h"

AGPPlayerController::AGPPlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    myIP = GetPlayerNetworkAddress();

    bReplicates = true;
}

void AGPPlayerController::BeginPlay()
{    
	if (GEngine)
	{
		/*UGameUserSettings* Settings = GEngine->GetGameUserSettings();
		FIntPoint Resolution = Settings->GetScreenResolution();
		Settings->RequestResolutionChange(Resolution.X, Resolution.Y, EWindowMode::WindowedFullscreen, false);*/
	}
}

// Handles replication of properties to clients in multiplayer!
void AGPPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Replicate IP to all clients.
    DOREPLIFETIME(AGPPlayerController, myIP);
}


// The following code is taken from the replication wiki. Details how to update a boolean property on the server from a client.

void AGPPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (Role == ROLE_Authority && IsPrimaryPlayer())
    {
        IsServerPlayer = true;
        InputComponent->BindAction("TriggerRescan", IE_Pressed, this, &AGPPlayerController::OnRequestRescan);
    }
    else
    {
        // set up gameplay key bindings
        InputComponent->BindAxis("MoveForward", this, &AGPPlayerController::MoveForward);
        InputComponent->BindAxis("MoveRight", this, &AGPPlayerController::MoveRight);
        InputComponent->BindAxis("Turn", this, &AGPPlayerController::AddControllerYawInput);
        InputComponent->BindAxis("LookUp", this, &AGPPlayerController::AddControllerPitchInput);

        InputComponent->BindAction("Jump", IE_Pressed, this, &AGPPlayerController::OnStartJump);
        InputComponent->BindAction("Jump", IE_Released, this, &AGPPlayerController::OnStopJump);

        InputComponent->BindAction("Fire", IE_Pressed, this, &AGPPlayerController::OnFire);

        InputComponent->BindAction("RemoteBombPlant", IE_Pressed, this, &AGPPlayerController::OnBombLaunch);
        InputComponent->BindAction("RemoteBombDetonate", IE_Pressed, this, &AGPPlayerController::OnBombDetonate);
    }
}

void AGPPlayerController::JoinTeam0()
{
	if (GetCharacter() != NULL && !InTeam)
	{
		AGPCharacter* Char = Cast<AGPCharacter>(GetCharacter());
		Char->JoinTeam(0);
		Char->SetMaterial(0);
        InTeam = true;
	}
}

void AGPPlayerController::JoinTeam1()
{
	if (GetCharacter() != NULL && !InTeam)
	{
		AGPCharacter* Char = Cast<AGPCharacter>(GetCharacter());
		Char->JoinTeam(1);
		Char->SetMaterial(1);
        InTeam = true;
    }
}

void AGPPlayerController::MoveForward(float Value)
{
	// Check that the game is not paused before allowing movement
	AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
	if (GetCharacter() != NULL)
	{
		if ((((AGPCharacter*)GetCharacter())->Health > 0) && (Value != 0.0f) && (gs->GetState() == 1))
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
}

void AGPPlayerController::MoveRight(float Value)
{
	AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
	if (GetCharacter() != NULL)
	{
		if ((((AGPCharacter*)GetCharacter())->Health > 0) && (Value != 0.0f) && (gs->GetState() == 1))
		{
			// find out which way is right
			const FRotator Rotation = GetControlRotation();
			const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
			// add movement in that direction
			GetCharacter()->AddMovementInput(Direction, Value);
		}
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
	AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
	if (GetCharacter() != NULL && gs->GetState() == 1)
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

void AGPPlayerController::OnBombLaunch()
{
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnBombLaunch();
	}
}

void AGPPlayerController::OnBombDetonate()
{
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnBombDetonate();
	}
}

void AGPPlayerController::OnRequestRescan()
{
	// Only server may rescan and access the game mode.
	if (Role == ROLE_Authority) {
		// TODO: Need to ensure this cast will succeed.
		//AGPGameMode *gmode = Cast<AGPGameMode>(GetWorld()->GetAuthGameMode());
		//if (gmode != NULL) {
		//	gmode->wantScan = true;
		//}
		
		// Repurpose shift-k for unpause purposes.
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Setting game state"));
		AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
		gs->SetState(1);
		gs->SetWaitingForRescan(false);
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
