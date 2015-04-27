// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPGameState.h"
#include "UnrealNetwork.h"
#include "GPPlayerState.h"
#include "GPGameMode.h"

AGPGameState::AGPGameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TeamPlayerCount[0] = 0;
	TeamPlayerCount[1] = 0;
	gameState = 1;
	flagLeader = 0;
}

int8 AGPGameState::GetSetTeam()
{
	if (TeamPlayerCount[0] == TeamPlayerCount[1])
	{
		UE_LOG(LogTemp, Warning, TEXT("Added player to team 1!"));
		TeamPlayerCount[0]++;
		return 0;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Added player to team 2!"));
		TeamPlayerCount[1]++;
		return 1;
	}
}

void AGPGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone so that we don't have to broadcast server functions
	DOREPLIFETIME(AGPGameState, gameState);
	DOREPLIFETIME(AGPGameState, flagLeader);
	DOREPLIFETIME(AGPGameState, team1Flags);
	DOREPLIFETIME(AGPGameState, team0Flags);
}

void AGPGameState::SetState(int32 newState)
{
	gameState = newState;
}

int32 AGPGameState::GetState()
{
	return gameState;
}

int32 AGPGameState::GetFlagLeader()
{
	return flagLeader;
}

void AGPGameState::UpdateFlagLeader()
{
	UWorld * const World = GetWorld();
	AGPGameMode * gm = Cast<AGPGameMode>(World->GetAuthGameMode());
	if (gm == NULL || !gm)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GameMode null"));
	}
	else
	{
		int32 team0 = 0;
		int32 team1 = 0;
		TArray<class APlayerState*>PStates = PlayerArray;
		if (PStates[0])
		{
			for (int32 i = 0; i < PStates.Num(); i++)
			{
				AGPPlayerState* PState = (AGPPlayerState*)PStates[i];
				int32 numFlags = PState->GetNumFlags();
				int8 team = PState->Team;
				if (team == 0)
				{
					team0 += numFlags;
				}
				else if (team == 1)
				{
					team1 += numFlags;
				}
			}
			team0Flags = team0;
			team1Flags = team1;
			if (team0Flags > team1Flags)
			{
				flagLeader = 0;
				if (team0Flags >= 1)
				{
					gm->EndGame(0);
				}
			}
			else
			{
				flagLeader = 1;
				if (team1Flags >= 1)
				{
					gm->EndGame(1);
				}
			}
		}
	}
}

int32 AGPGameState::GetTeamFlags(int32 team)
{
	if (team == 0)
	{
		return team0Flags;
	}
	else if (team == 1)
	{
		return team1Flags;
	}
	else
	{
		return 0;
	}
}
