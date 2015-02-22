// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPProjectorPlayerPawn.h"


AGPProjectorPlayerPawn::AGPProjectorPlayerPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create a CameraComponent 
	MapCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	//MapCameraComponent->AttachParent = GetCapsuleComponent();
	// Position the camera a bit above the eyes
	//MapCameraComponent->
	MapCameraComponent->RelativeLocation = FVector(0, 0, 50.0f);
	// Allow the pawn to control rotation.
	MapCameraComponent->bUsePawnControlRotation = false;

	RootComponent = MapCameraComponent;
}