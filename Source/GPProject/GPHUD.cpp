// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPHUD.h"
#include "GPCharacter.h"

AGPHUD::AGPHUD(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set the crosshair texture
    static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("Texture2D'/Game/crosshair.crosshair'"));
    CrosshairTex = CrosshairTexObj.Object;

	// Grab a default font
	static ConstructorHelpers::FObjectFinder<UFont> DefaultFontObj(TEXT("Font'/Game/Fonts/DroidSans'"));
	DefaultFont = DefaultFontObj.Object;
}

void AGPHUD::DrawHUD()
{
    Super::DrawHUD();
    // Draw very simple crosshair
    // find center of the Canvas
    const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
    // offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
    const FVector2D CrosshairDrawPosition((Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
        (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)));
    // draw the crosshair
    FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
    TileItem.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(TileItem);

	// This is all dubious!
	//AActor *Owner = this->GetOwner();

	//if (Owner && Owner->IsA<AGPCharacter>()) {
	//	AGPCharacter *PlayerOwner = static_cast<AGPCharacter *>(Owner);

	//	Canvas->DrawText(DefaultFont, FString::SanitizeFloat(PlayerOwner->Health), 10.0f, 10.0f);
	//}
	//else {
	//	Canvas->DrawText(DefaultFont, TEXT("IDK"), 10.0f, 10.0f);
	//}
	if (GetOwningPawn() != NULL) {
		AGPCharacter *PlayerOwner = dynamic_cast<AGPCharacter *>(GetOwningPawn());
		Canvas->DrawText(DefaultFont, FString::SanitizeFloat(PlayerOwner->Health), 10.0f, 10.0f);
		Canvas->DrawText(DefaultFont, FString::SanitizeFloat(PlayerOwner->GetActorLocation().X), 10.0f, 50.0f);
		Canvas->DrawText(DefaultFont, FString::SanitizeFloat(PlayerOwner->GetActorLocation().Y), 10.0f, 100.0f);
	}
}


