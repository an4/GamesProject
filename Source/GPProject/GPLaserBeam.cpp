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

	// Instance on clients.
	bNetLoadOnClient = true;
	bReplicates = false;
	bReplicateMovement = false;

}

// Called when the game starts or when spawned
void AGPLaserBeam::BeginPlay()
{
	FVector origin = FVector();
	FVector boxExtent = FVector();
	GetActorBounds(false, origin, boxExtent);
	SetLifeSpan(InitialLifeSpan);		//Really shouldn't be needed, but it apparently is.
}

void AGPLaserBeam::SetLengthAndPitch(float Length, float Pitch)
{
	if (Role == ROLE_Authority)
	{
		Scale = FVector(0.03f, Length / 200, 0.03f);
		Angle.Pitch = Pitch;
		Angle.Yaw = 0;
		Angle.Roll = 0;

		SetActorScale3D(Scale);
		SetActorRotation(GetActorRotation() + Angle);
	}
}

void AGPLaserBeam::OnRep_Scale()
{
	SetActorScale3D(Scale);
}

void AGPLaserBeam::OnRep_Angle()
{
	SetActorRotation(GetActorRotation() + Angle);
}

void AGPLaserBeam::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate scale to all clients.
	DOREPLIFETIME(AGPLaserBeam, Scale);
	DOREPLIFETIME(AGPLaserBeam, Angle);
}
