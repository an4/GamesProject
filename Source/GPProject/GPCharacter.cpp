// Fill out your copyright notice in the Description page of Project Settings.
#include "GPProject.h"
#include "GPProjectile.h"
#include "GPCharacter.h"
#include "UnrealNetwork.h"

AGPCharacter::AGPCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Create a CameraComponent 
    FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
    // Position the camera a bit above the eyes
    FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 50.0f + BaseEyeHeight);
    // Allow the pawn to control rotation.
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    FirstPersonMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("FirstPersonMesh"));
    FirstPersonMesh->SetOnlyOwnerSee(true);         // only the owning player will see this mesh
    FirstPersonMesh->AttachParent = FirstPersonCameraComponent;
    FirstPersonMesh->bCastDynamicShadow = false;
    FirstPersonMesh->CastShadow = false;

    // everyone but the owner can see the regular body mesh
    GetMesh()->SetOwnerNoSee(true);
}

float AGPCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// TODO: Implement this properly ourselves (with damage type handlers!)
	// For now, simply call the super method to do anything that might be necessary, and ignore any checks.
	Health -= DamageAmount;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("We took damage!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Health).Append(" HP"));
		
		if (Health <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("We died! Oh noes!"));
			Respawn();
		}
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AGPCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("We are using GPCharacter!"));
    }

	// Set starting health
	Health = 100.0f;
}

void AGPCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("Setting up input"));
    // set up gameplay key bindings
    //InputComponent->BindAxis("MoveForward", this, &AGPCharacter::MoveForward);
    //InputComponent->BindAxis("MoveRight", this, &AGPCharacter::MoveRight);
    //InputComponent->BindAxis("Turn", this, &AGPCharacter::AddControllerYawInput);
    //InputComponent->BindAxis("LookUp", this, &AGPCharacter::AddControllerPitchInput);

    InputComponent->BindAction("Jump", IE_Pressed, this, &AGPCharacter::OnStartJump);
    InputComponent->BindAction("Jump", IE_Released, this, &AGPCharacter::OnStopJump);

    //InputComponent->BindAction("Fire", IE_Pressed, this, &AGPCharacter::OnFire);
}

void AGPCharacter::Respawn()
{
    /*bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    FirstPersonCameraComponent->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f), false);*/
    SetActorLocationAndRotation(FVector(380.0f, 0.0f, 112.0f), FRotator(0.0f, 0.0f, 180.0f), false);
    //GetRootComponent()->SetWorldLocationAndRotation(FVector(380.0f, 0.0f, 112.0f), FQuat(FRotator(0.0f, 0.0f, 0.0f)));
    /*bUseControllerRotationPitch = true;
    bUseControllerRotationRoll = true;
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;*/

    Health = 100;
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("We have been respawned!"));
    }
}

//void AGPCharacter::MoveForward(float Value)
//{
//	// TODO: Health test - forward disabled when health gone
//    if ((Controller != NULL) && (Value != 0.0f) && (Health > 0.0f))
//    {
//        // find out which way is forward
//        FRotator Rotation = Controller->GetControlRotation();
//        // Limit pitch when walking or falling
//        if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
//        {
//            Rotation.Pitch = 0.0f;
//        }
//        // add movement in that direction
//        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
//        AddMovementInput(Direction, Value);
//    }
//}
//
//void AGPCharacter::MoveRight(float Value)
//{
//    if ((Controller != NULL) && (Value != 0.0f))
//    {
//        // find out which way is right
//        const FRotator Rotation = Controller->GetControlRotation();
//        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
//        // add movement in that direction
//        AddMovementInput(Direction, Value);
//    }
//}

void AGPCharacter::OnStartJump()
{
    bPressedJump = true;
}

void AGPCharacter::OnStopJump()
{
    bPressedJump = false;
}

// Abstract fire conditions to a function, as if the client attempts to fire erroneously they will be dropped!
bool AGPCharacter::CanFire()
{
	return Health > 0.0f;
}

void AGPCharacter::OnFire()
{
	// WARNING: This condition -MUST- match that in validate, else the client may be disconnected!
	if (CanFire())
	{
		ServerOnFire();
	}
}

bool AGPCharacter::ServerOnFire_Validate()
{
	// Only allow the character to fire if they have health.
	return CanFire();
}

void AGPCharacter::ServerOnFire_Implementation()
{
	// If we have been validated by the server, then we need to broadcast the fire event to all clients.
	if (Role == ROLE_Authority) {
		// TODO: Move this client side and validate?
		FVector CameraLoc;
		FRotator CameraRot;
		GetActorEyesViewPoint(CameraLoc, CameraRot);

		BroadcastOnFire(CameraLoc, CameraRot);
	}
}

void AGPCharacter::BroadcastOnFire_Implementation(FVector CameraLoc, FRotator CameraRot)
{
	if (ProjectileClass != NULL)
	{
		// Get the camera transform
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the camera to find the final muzzle position
		FVector const MuzzleLocation = CameraLoc + FTransform(CameraRot).TransformVector(MuzzleOffset);
		FRotator MuzzleRotation = CameraRot;
		MuzzleRotation.Pitch += 10.0f;          // skew the aim upwards a bit
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			// spawn the projectile at the muzzle
			AGPProjectile* const Projectile = World->SpawnActor<AGPProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// find launch direction
				FVector const LaunchDir = MuzzleRotation.Vector();
				Projectile->InitVelocity(LaunchDir);
			}
		}
	}
}

// Handles replication of properties to clients in multiplayer!
void AGPCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate health to all clients.
	DOREPLIFETIME(AGPCharacter, Health);
}
