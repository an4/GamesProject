// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPBuilding.h"

AGPBuilding::AGPBuilding(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    DummyRoot = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("RootBoxComponent"));
    RootComponent = DummyRoot;

    BuildingMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubemeshpath(TEXT("StaticMesh'/Game/Meshes/GP_Cube.GP_Cube'"));
	if (cubemeshpath.Object)
		BuildingMesh->SetStaticMesh(cubemeshpath.Object);

	BuildingMesh->AttachTo(RootComponent);
}

void AGPBuilding::BeginPlay()
{
	GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Blue, TEXT("BeginPlay for the block"));
}
