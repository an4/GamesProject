// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GPRemoteBomb.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPRemoteBomb : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, category = RemoteBomb)
	UBoxComponent* BombCollisionComp;

	AGPRemoteBomb(const FObjectInitializer& ObjectInitializer);
	
	public:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		UProjectileMovementComponent* BombProjectileMovement;

		/** inits velocity of the projectile in the shoot direction */
		void InitVelocity(const FVector& ShootDirection);
		UFUNCTION()
		void OnHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
		UFUNCTION()
		void Explode();

		/** Projectile class to spawn */
		UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AGPProjectile> ProjectileClass;

        UPROPERTY(EditDefaultsOnly, Category = Sounds)
        USoundCue* BombDetonateSound;

        UPROPERTY(EditDefaultsOnly, Category = Sounds)
        USoundCue* BombDropSound;
};
