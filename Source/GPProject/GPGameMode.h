// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "GPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPGameMode : public AGameMode
{
    GENERATED_BODY()

    AGPGameMode(const class FObjectInitializer& ObjectInitializer);
        
    virtual void StartPlay() override;
};

