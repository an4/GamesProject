// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "GPHUD.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPHUD : public AHUD
{
	GENERATED_BODY()
    AGPHUD(const FObjectInitializer& ObjectInitializer);

    /** Primary draw call for the HUD */
    virtual void DrawHUD() override;
	
private:
    /** Crosshair asset pointer */
    UTexture2D* CrosshairTex;
	UFont* DefaultFont;
	
};
