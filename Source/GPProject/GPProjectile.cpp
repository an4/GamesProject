// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPProjectile.h"

AGPProjectile::AGPProjectile(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Use a sphere as a simple collision representation
    CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("BoxComp"));
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->InitSphereRadius(15.0f);
    CollisionComp->OnComponentHit.AddDynamic(this, &AGPProjectile::OnHit);
    RootComponent = CollisionComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 5000.f;
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

	//Set the texture of the projectile
	//Should have 6 different colours (Red, Blue, Orange, Yellow, White, Green)
	ProjectileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshpath(TEXT("StaticMesh'/Game/Meshes/GP_Cube.GP_Cube'"));
	if (meshpath.Object) {
		ProjectileMesh->SetStaticMesh(meshpath.Object);
	}
	ProjectileMesh->AttachTo(RootComponent);

    static ConstructorHelpers::FObjectFinder<USoundCue> HitSoundCueLoader(TEXT("SoundCue'/Game/Audio/OnHit_Cue.OnHit_Cue'"));
    OnHitSound = HitSoundCueLoader.Object;
}

void AGPProjectile::InitVelocity(const FVector& ShootDirection, const FVector& ActorVelocity)
{
    if (ProjectileMovement)
    {
		float ActorVel = 0;
		ActorVel = (ShootDirection.X * ActorVelocity.X + ShootDirection.Y * ActorVelocity.Y + ShootDirection.Z * ActorVelocity.Z);
        // set the projectile's velocity to the desired direction
		float iniSpeed = 2*ProjectileMovement->InitialSpeed;
		if (ActorVel > 0)
		{
			iniSpeed += ActorVel;
		}
        ProjectileMovement->Velocity = (ShootDirection * iniSpeed);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::SanitizeFloat(ActorVel));
	}
}

void AGPProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // Play Sound
    this->PlaySoundOnActor(OnHitSound, 0.3f, 0.3f);
    
    if (OtherActor && (OtherActor != this) && OtherComp && Role == ROLE_Authority)
	{
		OtherComp->AddImpulseAtLocation(ProjectileMovement->Velocity * 100.0f, Hit.ImpactPoint);

		if (OtherActor->IsA(AGPCharacter::StaticClass())) {
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
				((AGPCharacter*)OtherActor)->ServerTakeDamage(damage, *DamageEvent, somethingelse, this);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Somethingelse is NULL"));
			}
		}
	}
}