// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPHUD.h"
#include "GPCharacter.h"
#include "GPGameState.h"
#include "GPPlayerState.h"

AGPHUD::AGPHUD(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	//// Grab a default font
	static ConstructorHelpers::FObjectFinder<UFont> DefaultFontObj(TEXT("Font'/Game/Fonts/DroidSans'"));
	DefaultFont = DefaultFontObj.Object;
}

void AGPHUD::DrawHUD()
{
    Super::DrawHUD();
}
