// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPProjectile.h"

AGPProjectile::AGPProjectile(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Use a sphere as a simple collision representation
    CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->InitSphereRadius(15.0f);
    CollisionComp->OnComponentHit.AddDynamic(this, &AGPProjectile::OnHit);
    RootComponent = CollisionComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 0.9f;

    // Die after 3 seconds by default
    InitialLifeSpan = 3.0f;

	// Instance on clients.
	bNetLoadOnClient = true;
	bReplicates = false;
	bReplicateMovement = false;
	hitWall = false;
}

void AGPProjectile::InitVelocity(const FVector& ShootDirection)
{
    if (ProjectileMovement)
    {
        // set the projectile's velocity to the desired direction
        ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
    }
}

void AGPProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && OtherComp && Role == ROLE_Authority)
	{
		OtherComp->AddImpulseAtLocation(ProjectileMovement->Velocity * 100.0f, Hit.ImpactPoint);

		if (OtherActor->IsA(AGPBuilding::StaticClass()) || OtherActor->IsA(AStaticMeshActor::StaticClass())) {
			hitWall = true;
		} 
		else if (OtherActor->IsA(AGPCharacter::StaticClass())) {
			if (hitWall == true) {
				/*((AGPPlayerState*)((AGPCharacter*)OtherActor)->PlayerState)->Team !=
					((AGPPlayerState*)((AGPCharacter*)GetOwner())->PlayerState)->Team*/
				
				if ((AGPPlayerState*)((AGPCharacter*)OtherActor)->PlayerState == NULL)
				{
					UE_LOG(LogTemp, Warning, TEXT("They don't have a playerstate!"));
				}
				if ((AGPPlayerState*)((AGPCharacter*)GetOwner())->PlayerState == NULL)
				{
					UE_LOG(LogTemp, Warning, TEXT("Our owner doesn't have a playerstate!"));
				}


				// Damage the other actor! TODO: Is there a proper way to use the damage system in UE4?
				const float damage = 5.0f;
				// Uuuh pointers? Hmm... TODO: nullptr -> subclass of UDamageType
				FPointDamageEvent* DamageEvent = new FPointDamageEvent(damage, Hit, NormalImpulse, nullptr);

                APawn* something = ((APawn*)GetOwner());
                AController* somethingelse = something->GetController();
                if (somethingelse) {
                    OtherActor->TakeDamage(damage, *DamageEvent, somethingelse, this);
                }
                else {
                    UE_LOG(LogTemp, Warning, TEXT("Somethingelse is NULL"));
                }
			}
		}
	}
}