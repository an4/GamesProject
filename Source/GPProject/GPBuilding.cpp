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
	static ConstructorHelpers::FObjectFinder<UMaterial> texLegoPath(TEXT("Material'/Game/Materials/texLego.texLego'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> texBookPath(TEXT("Material'/Game/Materials/texBook.texBook'"));
	if (cubemeshpath.Object)
	{
		BuildingMesh->SetStaticMesh(cubemeshpath.Object);
		texLego = texLegoPath.Object;
		texBook = texBookPath.Object;
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CHECK"));

	// Set the material here..
	if (Scale.Z < Scale.Y*0.5 && Scale.Z < Scale.Y*0.5) {
		BuildingMesh->SetMaterial(0, texBook);
	}
	else {
		BuildingMesh->SetMaterial(0, texLego);
	}

	SetActorScale3D(Scale);
}

void AGPBuilding::SetScale(FVector AbsoluteScale)
{
	if (Role == ROLE_Authority)
	{
		Scale = AbsoluteScale;

		// Set the material here..
		if (AbsoluteScale.Z < AbsoluteScale.Y*0.5 && AbsoluteScale.Z < AbsoluteScale.Y*0.5) {
			BuildingMesh->SetMaterial(0, texBook);
		}
		else {
			BuildingMesh->SetMaterial(0, texLego);
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


