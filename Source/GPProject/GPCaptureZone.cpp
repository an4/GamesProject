// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPCaptureZone.h"
#include "GPCharacter.h"
#include "GPPlayerState.h"

AGPCaptureZone::AGPCaptureZone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// The pickup is valid when it is created.
	bIsActive = true;

	// Create the root SphereComponent to handle the pickup's collision
	BaseCollisionComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("BaseSphereComponent"));

	// Set the SphereComponent as the root component.
	RootComponent = BaseCollisionComponent;

	bReplicates = true;

	BaseCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGPCaptureZone::OnOverlapBegin);
}

void AGPCaptureZone::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AGPCaptureZone, cpTeam);
}

void AGPCaptureZone::Init(int8 Team) {
	cpTeam = Team;
	TArray<UActorComponent*> components;
	USpotLightComponent* spotlight = NULL;
	GetComponents(components);
	for (int32 i = 0; i < components.Num(); i++)
	{
		// Find the spotlight component
		UActorComponent* comp = components[i];
		if (components[i]->GetName() == "SpotLight")
		{
			// Set the intensity on all clients so everyone can see we have a flag
			spotlight = Cast<USpotLightComponent>(comp);
			break;
		}
	}
	if (spotlight != NULL) {
		if (cpTeam == 0) {
			spotlight->SetLightColor(FLinearColor(0.f, 255.f, 0.f));
		}
		else if (cpTeam == 1) {
			spotlight->SetLightColor(FLinearColor(255.f, 0.f, 0.f));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("cpTeam is not set"));
		}
	}
}

void AGPCaptureZone::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check we have an actor
	AGPCharacter* const currentActor = Cast<AGPCharacter>(OtherActor);
	if (currentActor)
	{
		// Check that actor has a state
		AGPPlayerState* PState = (AGPPlayerState*)currentActor->PlayerState;
		if (PState)
		{
			FString str;
			if (PState->GetHasFlag() == true)
			{
				str = "Has flag";
			}
			else
			{
				str = "Does not have flag";
			}
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, str);
			// Check we have the flag
			if (PState->GetHasFlag() && PState->Team == cpTeam) {
				// Tell actor to cap it!
				currentActor->OnFlagCapture();
			}
		}
	}
}

// Only call this from a client when they join the game, to update all the materials for them
void AGPCaptureZone::ClientOnlySetColor() {
	TArray<UActorComponent*> components;
	USpotLightComponent* spotlight = NULL;
	GetComponents(components);
	for (int32 i = 0; i < components.Num(); i++)
	{
		// Find the spotlight component
		UActorComponent* comp = components[i];
		if (components[i]->GetName() == "SpotLight")
		{
			// Set the intensity on all clients so everyone can see we have a flag
			spotlight = Cast<USpotLightComponent>(comp);
			break;
		}
	}
	if (spotlight != NULL) {
		if (cpTeam == 0) {
			spotlight->SetLightColor(FLinearColor(0.f, 255.f, 0.f));
		}
		else if (cpTeam == 1) {
			spotlight->SetLightColor(FLinearColor(255.f, 0.f, 0.f));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("cpTeam is not set"));
		}
	}
}