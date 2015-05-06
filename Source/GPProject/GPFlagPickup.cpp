// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPCharacter.h"
#include "GPFlagPickup.h"
#include "GPPlayerState.h"

AGPFlagPickup::AGPFlagPickup(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("StaticMesh'/Game/ExampleContent/Input_Examples/Meshes/SM_Toy_Tree.SM_Toy_Tree'"));
    
    // static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("StaticMesh'/Game/Meshes/GP_Cube.GP_Cube'"));

	static ConstructorHelpers::FObjectFinder<UMaterial> Material1(TEXT("Material'/Game/Materials/Red.Red'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> Material2(TEXT("Material'/Game/Materials/Green.Green'"));
	if (Material1.Object != NULL)
	{
		RedMaterial = (UMaterial*)Material1.Object;
		UE_LOG(LogTemp, Warning, TEXT("RedMaterial has a value"));
	}
	if (Material2.Object != NULL)
	{
		GreenMaterial = (UMaterial*)Material2.Object;
		UE_LOG(LogTemp, Warning, TEXT("GreenMaterial has a value"));
	}
	
    if (StaticMesh.Object) {
        PickupMesh->SetStaticMesh(StaticMesh.Object);
    }
	/*
    PickupMesh->SetMaterial(0, Material_Green.Object);*/
	static ConstructorHelpers::FObjectFinder<UMaterial> Material3(TEXT("Material'/Game/Materials/TextFacingCamera.TextFacingCamera'"));
	if (Material3.Object != NULL)
	{
		TextMaterial = (UMaterial*)Material3.Object;
	}

    BaseCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGPFlagPickup::OnOverlapBegin);
	BaseCollisionComponent->InitSphereRadius(100.0f);
	BaseCollisionComponent->SetSphereRadius(100.0f);
	textRender = ObjectInitializer.CreateDefaultSubobject<UTextRenderComponent>(this, TEXT("TimeToRespawn"));
	textRender->AttachTo(RootComponent);
	textRender->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 100.f), FRotator(0.f, 90.f, 0.f));
	//textRender->SetMaterial(0, UMaterialInstanceDynamic::Create(TextMaterial, this));
	//textRender->SetWorldSize(100.f);
	textRender->SetText("");
	timeAlive = 0.0f;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
}

void AGPFlagPickup::Init(int8 Team, bool dropped) {
	flagTeam = Team;
	ServerSetMaterial(Team);
	if (dropped)
	{
		ServerSetLight(Team, 100000.f);
	}
	wasDropped = dropped;
}

bool AGPFlagPickup::ServerSetMaterial_Validate(int8 Team)
{
	return true;
}

void AGPFlagPickup::ServerSetMaterial_Implementation(int8 Team)
{
	// If we have been validated by the server, then we need to broadcast the change team to all clients.
	if (Role == ROLE_Authority) {
		BroadcastSetMaterial(Team);
	}
}

void AGPFlagPickup::BroadcastSetMaterial_Implementation(int8 Team) {
	if (Team == 0) {
		PickupMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(GreenMaterial, this));
	}
	else
	{
		PickupMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(RedMaterial, this));
	}
}

bool AGPFlagPickup::ServerSetLight_Validate(int8 Team, float val)
{
	return true;
}

void AGPFlagPickup::ServerSetLight_Implementation(int8 Team, float val)
{
	// If we have been validated by the server, then we need to broadcast the change team to all clients.
	if (Role == ROLE_Authority) {
		BroadcastSetLight(Team, val);
	}
}

void AGPFlagPickup::BroadcastSetLight_Implementation(int8 Team, float val) {
	TArray<UActorComponent*> components;
	GetComponents(components);
	for (int32 i = 0; i < components.Num(); i++)
	{
		// Find the spotlight component
		UActorComponent* comp = components[i];
		if (components[i]->GetName() == "FlagSpotLight")
		{
			// Set the intensity on all clients so everyone can see we have a flag
			USpotLightComponent * spotlight = Cast<USpotLightComponent>(comp);
			if (spotlight) {
				if (Team == 0)
				{
					spotlight->SetLightColor(FLinearColor(0.f, 255.f, 0.f, 1.f));
				}
				else
				{
					spotlight->SetLightColor(FLinearColor(255.f, 0.f, 0.f, 1.f));
				}
				spotlight->SetIntensity(val);
			}
		}
	}
}

// Only call this from a client when they join the game, to update all the materials for them
void AGPFlagPickup::ClientOnlySetMaterial() {
	if (flagTeam == 0) {
		PickupMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(GreenMaterial, this));
	}
	else if (flagTeam == 1) {
		PickupMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(RedMaterial, this));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("flagTeam is not set"));
	}
}

void AGPFlagPickup::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AGPFlagPickup, flagTeam);
	DOREPLIFETIME(AGPFlagPickup, textRender);
}

void AGPFlagPickup::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGPCharacter* const currentActor = Cast<AGPCharacter>(OtherActor);
	if (currentActor)
	{
		AGPPlayerState* PState = (AGPPlayerState*)currentActor->PlayerState;
		if (PState) {
			if (currentActor->CanPickupFlag() && PState->Team == flagTeam) {
                Super::playSound();

				//GetWorld()->DestroyActor(this, true);

				// Moved flag spawning into the actor so that we can call the server to do it without needing flags to have an owner
				currentActor->OnFlagPickup(this);
			}
		}
	}
}

bool AGPFlagPickup::ServerMoveToSpawn_Validate()
{
	return true;
}

void AGPFlagPickup::ServerMoveToSpawn_Implementation()
{
	if (Role == ROLE_Authority)
	{
		BroadcastMoveToSpawn();
	}
}

void AGPFlagPickup::BroadcastMoveToSpawn_Implementation()
{
	FVector loc;
	if (flagTeam == 0) {
		loc = FVector(-2300.f, -3800.f, 0.f);
	}
	else
	{
		loc = FVector(2300.f, 3800.f, 0.f);
	}
	SetActorLocation(loc, false);
}

bool AGPFlagPickup::ServerSetText_Validate(int32 new_t)
{
	return true;
}

void AGPFlagPickup::ServerSetText_Implementation(int32 new_t)
{
	if (Role == ROLE_Authority)
	{
		BroadcastSetText(new_t);
	}
}

void AGPFlagPickup::BroadcastSetText_Implementation(int32 new_t)
{
	if (new_t <= 0)
	{
		textRender->SetText("");
	}
	else
	{
		FString int_part = FString::FromInt(new_t);
		FString str = FString("Returning to base in<br>").Append(int_part);
		textRender->SetText(str);
	}
}

void AGPFlagPickup::Tick(float DeltaSeconds)
{
	if (wasDropped)
	{
		timeAlive += DeltaSeconds;
		if (textRender) {
			int32 new_t = ceilf(timeToLive - timeAlive);
			// Get just the number
			FString c_t = textRender->Text.RightChop(24);
			int32 current_t = FCString::Atoi(*c_t);
			// Check that the time is different to avoid broadcasting on every tick
			if (new_t < current_t || (new_t == 10 && new_t > current_t))
			{
				ServerSetText(new_t);
			}
		}
		if (timeAlive >= timeToLive)
		{
			wasDropped = false;
			timeAlive = 0.0f;
			ServerMoveToSpawn();
			ServerSetLight(flagTeam, 0.f);
		}
	}
}

float AGPFlagPickup::GetTimeAlive()
{
	return timeAlive;
}

bool AGPFlagPickup::GetWasDropped()
{
	return wasDropped;
}

float AGPFlagPickup::GetTimeToLive()
{
	return timeToLive;
}