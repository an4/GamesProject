// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()
    
    // Constructor for AFPSCharacter
    AFPSCharacter(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;

	
    public:
        //handles firing
        UFUNCTION()
        void OnFire();

		// handles damage
		UFUNCTION()
		float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);

        /** Gun muzzle's offset from the camera location */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        FVector MuzzleOffset;

        /** Projectile class to spawn */
        UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class AFPSProjectile> ProjectileClass;

        /** First person camera */
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        UCameraComponent* FirstPersonCameraComponent;

        /** Pawn mesh: 1st person view (arms; seen only by self) */
        UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
        USkeletalMeshComponent* FirstPersonMesh;

		/** Property to store the character's health. */
		UPROPERTY(Replicated)
		float Health;

    protected:
        virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

        //handles moving forward/backward
        UFUNCTION()
        void MoveForward(float Val);
        //handles strafing
        UFUNCTION()
        void MoveRight(float Val);

        //sets jump flag when key is pressed
        UFUNCTION()
        void OnStartJump();
        //clears jump flag when key is released
        UFUNCTION()
        void OnStopJump();
	
};
