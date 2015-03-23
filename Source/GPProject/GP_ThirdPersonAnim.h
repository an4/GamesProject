// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "GP_ThirdPersonAnim.generated.h"

/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class GPPROJECT_API UGP_ThirdPersonAnim : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool bIsShooting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool bIsHit;

	UFUNCTION()
	void setDeathStatus(bool status);

	UFUNCTION()
	void setShootStatus(bool status);

	UFUNCTION()
	void setHitStatus(bool status);
	
	
};
