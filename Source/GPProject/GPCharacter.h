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

	void Tick(float DeltaSeconds) override;

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
        bool ServerOnBombDetonate_Validate();
        void ServerOnBombDetonate_Implementation();

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastOnBombDetonate();
        void BroadcastOnBombDetonate_Implementation();

		// handles damage
		//UFUNCTION()
		//float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerTakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);
		bool ServerTakeDamage_Validate(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);
		void ServerTakeDamage_Implementation(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);

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

		//Handles Flag Capture
		UFUNCTION()
		void OnFlagPickup(AGPFlagPickup * flag);

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerOnFlagPickup(AGPFlagPickup * flag);
		bool ServerOnFlagPickup_Validate(AGPFlagPickup * flag);
		void ServerOnFlagPickup_Implementation(AGPFlagPickup * flag);

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastOnFlagPickup();
		void BroadcastOnFlagPickup_Implementation();

		UFUNCTION()
		void OnFlagCapture();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerOnFlagCapture(int8 Team);
		bool ServerOnFlagCapture_Validate(int8 Team);
		void ServerOnFlagCapture_Implementation(int8 Team);

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastOnFlagCapture();
		void BroadcastOnFlagCapture_Implementation();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnFlag(FVector loc, int8 Team, bool wasDropped);
		bool ServerSpawnFlag_Validate(FVector loc, int8 Team, bool wasDropped);
		void ServerSpawnFlag_Implementation(FVector loc, int8 Team, bool wasDropped);

		UFUNCTION()
		bool CanPickupFlag();
		UFUNCTION()
		bool CanCaptureFlag();

        UFUNCTION()
        void OnHealthPickUp();

		// handle pausing
		//handles bomb detonation
		UFUNCTION()
		void SetPauseState();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetPauseState();
		bool ServerSetPauseState_Validate();
		void ServerSetPauseState_Implementation();

		UFUNCTION()
		void SetPauseStateOff();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetPauseStateOff();
		bool ServerSetPauseStateOff_Validate();
		void ServerSetPauseStateOff_Implementation();

        UPROPERTY(EditDefaultsOnly, Category = Sounds)
        USoundCue* ShotGunSound;

        UPROPERTY(EditDefaultsOnly, Category = Sounds)
        USoundCue* RespawnSound;

        UFUNCTION(BlueprintCallable, Category = "Health")
		float getHealth();

		UPROPERTY()
		bool resetFlag = false;

		UFUNCTION()
		void Spawn();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerRespawn(bool shallResetFlag);
		bool ServerRespawn_Validate(bool shallResetFlag);
		void ServerRespawn_Implementation(bool shallResetFlag);

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastRespawn();
		void BroadcastRespawn_Implementation();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerFinishRespawn();
		bool ServerFinishRespawn_Validate();
		void ServerFinishRespawn_Implementation();

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastFinishRespawn();
		void BroadcastFinishRespawn_Implementation();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetLightIntensity(float val);
		bool ServerSetLightIntensity_Validate(float val);
		void ServerSetLightIntensity_Implementation(float val);

		UFUNCTION(NetMulticast, Reliable)
		void BroadcastSetLightIntensity(float val);
		void BroadcastSetLightIntensity_Implementation(float val);

        UPROPERTY()
        int32 Ammo;

        UFUNCTION(BlueprintCallable, Category = "Ammo")
        int32 getAmmo();

        UFUNCTION()
        void OnAmmoPickUp(int32 Value);

    protected:
        virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
		FVector SpawnPoints[2];

        ////handles moving forward/backward
        //UFUNCTION()
        //void MoveForward(float Val);
        ////handles strafing
        //UFUNCTION()
        //void MoveRight(float Val);

	
};
