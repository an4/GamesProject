// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPBuilding.h"
#include "UnrealNetwork.h"

AGPBuilding::AGPBuilding(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    DummyRoot = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("RootBoxComponent"));
    RootComponent = DummyRoot;

    Building = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComponent")); 
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubemeshpath(TEXT("StaticMesh'/Game/Meshes/GP_Cube.GP_Cube'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubemeshpath2(TEXT("StaticMesh'/Game/Meshes/buildingCube.buildingCube'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> texLegoPath(TEXT("Material'/Game/Materials/texLego.texLego'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> texBookPath(TEXT("Material'/Game/Materials/texBook.texBook'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> texTruckPath(TEXT("Material'/Game/Materials/texTruck.texTruck'"));
	if (cubemeshpath.Object)
	{
		BuildingMesh = cubemeshpath.Object;
		BuildingMesh2 = cubemeshpath2.Object;
		texLego = texLegoPath.Object;
		texBook = texBookPath.Object;
		texTruck = texTruckPath.Object;
	}

	Building->AttachTo(RootComponent);

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
	//UE_LOG(LogTemp, Warning, TEXT("Scaling block client: %d"), Role == ROLE_Authority);

	//UE_LOG(LogTemp, Warning, TEXT("Scaling block client: %f %f %f"), Scale.X, Scale.Y, Scale.Z);

	// Set the material here..
	if (Scale.Z < Scale.Y*0.5 && Scale.Z < Scale.X*0.5 && Scale.Z < 2) {
		Building->SetStaticMesh(BuildingMesh);
		Building->SetMaterial(0, texBook);
	}
	else if (Scale.X > Scale.Y*2 || Scale.Y > Scale.X*2) {
		Building->SetStaticMesh(BuildingMesh);
		Building->SetMaterial(0, texLego);
	}
	else {
		Building->SetStaticMesh(BuildingMesh2);
		Building->SetMaterial(0, texTruck);
	}

	SetActorScale3D(Scale);
}

void AGPBuilding::SetScale(FVector AbsoluteScale)
{
	if (Role == ROLE_Authority)
	{
		Scale = AbsoluteScale;

		if (Scale.Z < Scale.Y*0.5 && Scale.Z < Scale.X*0.5 && Scale.Z < 2) {
			Building->SetStaticMesh(BuildingMesh);
			Building->SetMaterial(0, texBook);
		}
		else if (Scale.X > Scale.Y * 2 || Scale.Y > Scale.X * 2) {
			Building->SetStaticMesh(BuildingMesh);
			Building->SetMaterial(0, texLego);
		}
		else {
			Building->SetStaticMesh(BuildingMesh2);
			Building->SetMaterial(0, texTruck);
		}

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


