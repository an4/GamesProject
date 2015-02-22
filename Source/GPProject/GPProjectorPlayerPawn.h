// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "GPProjectorPlayerPawn.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPProjectorPlayerPawn : public APawn
{
	GENERATED_BODY()

	AGPProjectorPlayerPawn(const FObjectInitializer& ObjectInitializer);
	
public:
	/** Top-down map camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* MapCameraComponent;
	
	
};
