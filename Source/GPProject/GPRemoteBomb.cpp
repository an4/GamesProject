// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPRemoteBomb.h"
#include "GPProjectile.h"
#include "GPBuilding.h"
#include "GPCharacter.h"

AGPRemoteBomb::AGPRemoteBomb(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Use a box as a simple collision representation
	BombCollisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BoxComp"));
	BombCollisionComp->BodyInstance.SetCollisionProfileName("RemoteBomb");
	//BombCollisionComp->InitBoxExtent(FVector(5, 5, 5));
	BombCollisionComp->SetBoxExtent(FVector(5, 5, 5));
	BombCollisionComp->UpdateCollisionProfile();

	BombCollisionComp->OnComponentHit.AddDynamic(this, &AGPRemoteBomb::OnHit);
	RootComponent = BombCollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	BombProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	BombProjectileMovement->UpdatedComponent = BombCollisionComp;
	BombProjectileMovement->InitialSpeed = 300.f;
	BombProjectileMovement->MaxSpeed = 300.f;
	BombProjectileMovement->bRotationFollowsVelocity = false;
	BombProjectileMovement->bShouldBounce = true;
	BombProjectileMovement->Bounciness = 0.5f;

	// Die after 3 seconds by default
	InitialLifeSpan = 999.0f;

	// Instance on clients.
	bNetLoadOnClient = true;
	bReplicates = false;
	bReplicateMovement = false;

    static ConstructorHelpers::FObjectFinder<USoundCue> DetonateSoundCueLoader(TEXT("SoundCue'/Game/Audio/BombDetonate_Cue.BombDetonate_Cue'"));
    BombDetonateSound = DetonateSoundCueLoader.Object;

    static ConstructorHelpers::FObjectFinder<USoundCue> DropSoundCueLoader(TEXT("SoundCue'/Game/Audio/BombDrop_Cue.BombDrop_Cue'"));
    BombDropSound = DropSoundCueLoader.Object;
}

void AGPRemoteBomb::InitVelocity(const FVector& ShootDirection)
{
	if (BombProjectileMovement)
	{
		// set the projectile's velocity to the desired direction
		BombProjectileMovement->Velocity = ShootDirection * BombProjectileMovement->InitialSpeed;
	}
}

void AGPRemoteBomb::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && OtherComp && Role == ROLE_Authority)
	{
        // Play Sound
        this->PlaySoundOnActor(BombDropSound, 0.5f, 0.5f);
        
        // Disable movement once we hit the floor/a building
		if (OtherActor->IsA(AGPBuilding::StaticClass()) || OtherActor->IsA(AStaticMeshActor::StaticClass())) {
			BombProjectileMovement->Deactivate();
		}
		OtherComp->AddImpulseAtLocation(BombProjectileMovement->Velocity * 100.0f, Hit.ImpactPoint);
		/*if (OtherActor->IsA(AGPCharacter::StaticClass())) {
			// Stick to the actor
			BombProjectileMovement->Deactivate();
			OtherActor->GetRootPrimitiveComponent()->MoveIgnoreActors.Add(this);
			GetRootPrimitiveComponent()->MoveIgnoreActors.Add(OtherActor);
			EAttachLocation::Type AttachLocationType = EAttachLocation::SnapToTarget;
			AttachRootComponentToActor(OtherActor, NAME_None, AttachLocationType);
		}*/
	}
}

void AGPRemoteBomb::Explode()
{
	if (ProjectileClass != NULL)
	{
        // Play Sound
        this->PlaySoundOnActor(BombDetonateSound, 0.5f, 0.5f);
        
        UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this->GetOwner();
			SpawnParams.Instigator = Instigator;
			FVector MuzzleLocation = FVector(this->GetActorLocation());
			// spawn the projectile above the bomb
			for (int i = 0; i < 8; i++)
			{
				// Pitch, Roll, Yaw
				float pitch = FMath::FRandRange(15, 60);
				float roll = FMath::FRandRange(0, 359);
				FRotator MuzzleRotation = FRotator(pitch, roll, 0.0f);
				AGPProjectile* const Projectile = World->SpawnActor<AGPProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
				if (Projectile)
				{
					// find launch direction
					FVector const LaunchDir = MuzzleRotation.Vector();
					Projectile->InitVelocity(LaunchDir);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Failed to launch projectile"));
				}
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Failed to find class"));
	}
	AGPRemoteBomb::Destroy();
}