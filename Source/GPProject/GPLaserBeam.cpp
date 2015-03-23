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
	InitialLifeSpan = 0.2f;

	bReplicates = true;
	bReplicateMovement = true;
}

// Called when the game starts or when spawned
void AGPLaserBeam::BeginPlay()
{
	FVector origin = FVector();
	FVector boxExtent = FVector();
	GetActorBounds(false, origin, boxExtent);
	SetLifeSpan(InitialLifeSpan);		//Really shouldn't be needed, but it apparently is.
}

void AGPLaserBeam::SetScale(float Length)
{
	if (Role == ROLE_Authority)
	{
		Scale = FVector(0.03f, Length / 200, 0.03f);
		SetActorScale3D(Scale);
	}
}

void AGPLaserBeam::OnRep_Scale()
{
	SetActorScale3D(Scale);
}

void AGPLaserBeam::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate scale to all clients.
	DOREPLIFETIME(AGPLaserBeam, Scale);
}
