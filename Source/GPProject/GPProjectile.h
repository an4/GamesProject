// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GPBuilding.h"
#include "GPCharacter.h"
#include "GPPlayerState.h"
#include "GameFramework/Actor.h"
#include "GPProjectile.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPProjectile : public AActor
{
	GENERATED_BODY()
	/** Sphere collision component */
    UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
    USphereComponent* CollisionComp;

    AGPProjectile(const FObjectInitializer& ObjectInitializer);
	
    /** Projectile movement component */
    public:
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
        UProjectileMovementComponent* ProjectileMovement;

		// Flag to check if it has hit a wall
		UPROPERTY()
		bool hitWall;
	
        /** inits velocity of the projectile in the shoot direction */
        void InitVelocity(const FVector& ShootDirection);
        /** called when projectile hits something */
        UFUNCTION()
        void OnHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
