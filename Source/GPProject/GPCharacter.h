// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "GPCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPCharacter : public ACharacter
{
	GENERATED_BODY()
    
    // Constructor for AGPCharacter
    AGPCharacter(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	void SetupTeam();

	UFUNCTION()
	bool CanFire();
	
    public:
		UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Materials)
		UMaterial* GreenMaterial;
		
		UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Materials)
		UMaterial* RedMaterial;
		
		UFUNCTION()
		void SetMaterial(int8 Team);

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetMaterial(int8 Team);
		bool ServerSetMaterial_Validate(int8 Team);
		void ServerSetMaterial_Implementation(int8 Team);

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastSetMaterial(int8 Team);
		void BroadcastSetMaterial_Implementation(int8 Team);

		bool CanJoinTeam(int8 Team);

		UFUNCTION()
		void JoinTeam(int8 Team);

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerJoinTeam(int8 Team);
		bool ServerJoinTeam_Validate(int8 Team);
		void ServerJoinTeam_Implementation(int8 Team);

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastJoinTeam(int8 Team);
		void BroadcastJoinTeam_Implementation(int8 Team);

		//handles firing
        UFUNCTION()
        void OnFire();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerOnFire();
        bool ServerOnFire_Validate();
        void ServerOnFire_Implementation();

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastOnFire(FVector CameraLoc, FRotator CameraRot);
        void BroadcastOnFire_Implementation(FVector CameraLoc, FRotator CameraRot);

		//handles bomb launching
		UFUNCTION()
		void OnBombLaunch();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerOnBombLaunch();
        bool ServerOnBombLaunch_Validate();
        void ServerOnBombLaunch_Implementation();

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastOnBombLaunch(FVector CameraLoc, FRotator CameraRot);
        void BroadcastOnBombLaunch_Implementation(FVector CameraLoc, FRotator CameraRot);

		//handles bomb detonation
		UFUNCTION()
		void OnBombDetonate();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerOnBombDetonate();
        bool ServerOnBombDetonate_Validation();

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastOnBombDetonate();
        void BroadcastOnBombDetonate_Implementation();

		// handles damage
		UFUNCTION()
		float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);

		// handles points
		UFUNCTION()
		void IncreasePoints();

        /** Gun muzzle's offset from the camera location */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        FVector MuzzleOffset;

        /** Projectile class to spawn */
        UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class AGPProjectile> ProjectileClass;

		/** RemoteBomb class to spawn */
		UPROPERTY(EditDefaultsOnly, Category = Placeable)
		TSubclassOf<class AGPRemoteBomb> RemoteBombClass;

        /** First person camera */
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        UCameraComponent* FirstPersonCameraComponent;

        /** Pawn mesh: 1st person view (arms; seen only by self) */
        UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
        USkeletalMeshComponent* FirstPersonMesh;

		/** Property to store the character's health. */
		UPROPERTY(Replicated)
		float Health;

		// Property to store the character's points
		UPROPERTY()
		float Point;

		// List of mines
		UPROPERTY()
		TArray<AGPRemoteBomb*> RemoteBombList;

		// Check bombs have been planted before we try to explode them
		UPROPERTY()
		bool BombPlanted;

		// Maximum number of bombs
		UPROPERTY()
		int32 MaxBombs;

		//sets jump flag when key is pressed
		UFUNCTION()
		void OnStartJump();
		//clears jump flag when key is released
		UFUNCTION()
		void OnStopJump();

        UPROPERTY(Replicated)
        uint8 FlagsPickedUp;

        UFUNCTION()
        void OnFlagPickUp();

		// handle pausing
		//handles bomb detonation
		UFUNCTION()
		void SetPauseState();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetPauseState();

		UFUNCTION()
		void SetPauseStateOff();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetPauseStateOff();

        UPROPERTY(EditDefaultsOnly, Category = Sounds)
        USoundCue* ShotGunSound;

        UPROPERTY(EditDefaultsOnly, Category = Sounds)
        USoundCue* RespawnSound;

    protected:
        virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
		virtual void Respawn();
		FVector SpawnPoints[2];

        ////handles moving forward/backward
        //UFUNCTION()
        //void MoveForward(float Val);
        ////handles strafing
        //UFUNCTION()
        //void MoveRight(float Val);

	
};
