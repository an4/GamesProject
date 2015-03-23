// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPLaserBeam.h"
#include "UnrealNetwork.h"

// Sets default values
AGPLaserBeam::AGPLaserBeam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DummyRoot = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("RootBoxComponent"));
	RootComponent = DummyRoot;

	LaserMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubemeshpath(TEXT("StaticMesh'/Game/Meshes/GP_Cylinder.GP_Cylinder'"));
	if (cubemeshpath.Object)
		LaserMesh->SetStaticMesh(cubemeshpath.Object);

	LaserMesh->AttachTo(RootComponent);

	bReplicates = true;
	bReplicateMovement = true;
}

// Called when the game starts or when spawned
void AGPLaserBeam::BeginPlay()
{
	FVector origin = FVector();
	FVector boxExtent = FVector();
	GetActorBounds(false, origin, boxExtent);
}

void AGPLaserBeam::SetScale(float Length)
{
	if (Role == ROLE_Authority)
	{
		Scale = FVector(10 / 400, Length / 200, 10 / 400);
		SetActorScale3D(Scale);
	}
}

void AGPLaserBeam::OnRep_Scale()
{
	//GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Blue, *FString::Printf(TEXT("Scaling block client: %d"), Role == ROLE_Authority));
	SetActorScale3D(Scale);
}

// Handles replication of properties to clients in multiplayer!
void AGPLaserBeam::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate health to all clients.
	DOREPLIFETIME(AGPLaserBeam, Scale);
}
