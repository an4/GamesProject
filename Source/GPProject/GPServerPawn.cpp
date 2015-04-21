// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPServerPawn.h"


// Sets default values
AGPServerPawn::AGPServerPawn(const FObjectInitializer& ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Create a CameraComponent 
    //FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
    //FirstPersonCameraComponent->AttachParent = RootComponent;
    //FirstPersonCameraComponent->SetWorldLocation(FVector(0, 0, 2000));
    //FirstPersonCameraComponent->AddWorldRotation(FRotator(270, 0, 180));
}   

// Called when the game starts or when spawned
void AGPServerPawn::BeginPlay()
{
	Super::BeginPlay();
    SetActorLocation(FVector(0, 0, 5500));
}

// Called every frame
void AGPServerPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AGPServerPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}
