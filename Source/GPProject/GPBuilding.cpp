// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPBuilding.h"
#include "UnrealNetwork.h"

AGPBuilding::AGPBuilding(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    DummyRoot = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("RootBoxComponent"));
    RootComponent = DummyRoot;

    BuildingMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComponent")); 
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubemeshpath(TEXT("StaticMesh'/Game/Meshes/GP_Cube.GP_Cube'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> cubetexturepath1(TEXT("Material'/Game/Materials/M_Book.M_Book'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> cubetexturepath2(TEXT("Material'/Game/Materials/M_Book2.M_Book2'"));
	if (cubemeshpath.Object)
	{
		BuildingMesh->SetStaticMesh(cubemeshpath.Object);
		if (rand() % 2 == 0) {
			BuildingMesh->SetMaterial(0,cubetexturepath1.Object);
		}
		else {
			BuildingMesh->SetMaterial(0, cubetexturepath1.Object);
		}
	}

	BuildingMesh->AttachTo(RootComponent);

	bReplicates = true;
	bReplicateMovement = true;
}

void AGPBuilding::BeginPlay()
{
	FVector origin = FVector();
	FVector boxExtent = FVector();
	GetActorBounds(false, origin, boxExtent);
}

void AGPBuilding::OnRep_Scale()
{
	//GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Blue, *FString::Printf(TEXT("Scaling block client: %d"), Role == ROLE_Authority));
	SetActorScale3D(Scale);
}

void AGPBuilding::SetScale(FVector AbsoluteScale)
{
	if (Role == ROLE_Authority)
	{
		Scale = AbsoluteScale;
		SetActorScale3D(Scale);
	}
}

// Handles replication of properties to clients in multiplayer!
void AGPBuilding::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate health to all clients.
	DOREPLIFETIME(AGPBuilding, Scale);
}
