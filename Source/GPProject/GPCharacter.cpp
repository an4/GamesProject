// Fill out your copyright notice in the Description page of Project Settings.
#include "GPProject.h"
#include "GPProjectile.h"
#include "GPRemoteBomb.h"
#include "GPLaserBeam.h"
#include "GPCharacter.h"
#include "GPPlayerController.h"
#include "GPGameState.h"
#include "GPGameMode.h"
#include "UnrealNetwork.h"

AGPCharacter::AGPCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	Weapon = 0;
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

	LaserBeamClass = AGPLaserBeam::StaticClass();

    // everyone but the owner can see the regular body mesh
    GetMesh()->SetOwnerNoSee(true);

    // Set number of flags picked up to zero.
    FlagsPickedUp = 0;

    static ConstructorHelpers::FObjectFinder<USoundCue> GunShotSoundCueLoader(TEXT("SoundCue'/Game/Audio/GunShot_Cue.GunShot_Cue'"));
    ShotGunSound = GunShotSoundCueLoader.Object;

    static ConstructorHelpers::FObjectFinder<USoundCue> RespawnSoundCueLoader(TEXT("SoundCue'/Game/Audio/Respawn_Cue.Respawn_Cue'"));
    RespawnSound = RespawnSoundCueLoader.Object;
}

float AGPCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// TODO: Implement this properly ourselves (with damage type handlers!)
	// For now, simply call the super method to do anything that might be necessary, and ignore any checks.

	
	if (EventInstigator != GetController()) {
		AGPCharacter* otherPlayer;
		Health -= DamageAmount;

		if (DamageCauser->IsA(AStaticMeshActor::StaticClass()))			//Hitscan
		{
			otherPlayer = Cast<AGPCharacter, AActor>(DamageCauser);
			otherPlayer->IncreasePoints();
		}
		else if (DamageCauser->IsA(AGPProjectile::StaticClass()))	//Projectile
		{
			otherPlayer = Cast<AGPCharacter, AActor>(DamageCauser->GetOwner());
			otherPlayer->IncreasePoints();
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Health).Append(" HP"));

			if (Health <= 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("We died! Oh noes!"));
				Respawn();
			}
		}

		return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	else {
		return 0.0f;
	}
}

void AGPCharacter::IncreasePoints() {
	Point += 10.0f;
}

void AGPCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("A player has entered the game!"));
    }

	// Set starting health
	Health = 100.0f;
	Point = 0.0f;
	BombPlanted = false;
	MaxBombs = 5;
}

void AGPCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("Setting up input"));

    InputComponent->BindAction("Jump", IE_Pressed, this, &AGPCharacter::OnStartJump);
    InputComponent->BindAction("Jump", IE_Released, this, &AGPCharacter::OnStopJump);
}

void AGPCharacter::Respawn()
{
    // Play Sound
    this->PlaySoundOnActor(RespawnSound, 1.0f, 3.0f);

    SetActorLocationAndRotation(FVector(380.0f, 0.0f, 112.0f), FRotator::ZeroRotator, false);

    Health = 100;
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("We have been respawned!"));
    }
}

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
	AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(gs->GetState()));
	return (Health > 0.0f && gs->GetState() == 1);
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
	FVector MuzzleLocation = CameraLoc + FTransform(CameraRot).TransformVector(MuzzleOffset);
	FRotator MuzzleRotation = CameraRot;
	if (Weapon == 0 && ProjectileClass != NULL)
	{
		// Get the camera transform
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the camera to find the final muzzle position
		
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
                // Play Sound
                Projectile->PlaySoundOnActor(ShotGunSound, 0.2f, 0.5f);
                
                // find launch direction
				FVector const LaunchDir = MuzzleRotation.Vector();
				Projectile->InitVelocity(LaunchDir);
			}
		}
	}
	else if (Weapon == 1 && LaserBeamClass != NULL)
	{
		FHitResult OutHit;
		const FVector TraceDirection = MuzzleRotation.Vector();

		// Calculate the start location for trace  
		FVector StartTrace = FVector::ZeroVector;
		if (Controller)
		{
			FRotator UnusedRotation;
			Controller->GetPlayerViewPoint(StartTrace, UnusedRotation);

			// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start  
			StartTrace = StartTrace + TraceDirection * ((GetActorLocation() - StartTrace) | TraceDirection);
		}

		// Distance to trace  
		const float WeaponRange = 2000.0f;
		// Calculate endpoint of trace  
		const FVector EndTrace = StartTrace + TraceDirection * WeaponRange;

		// Setup the trace query  
		static FName FireTraceIdent = FName(TEXT("WeaponTrace"));
		FCollisionQueryParams TraceParams(FireTraceIdent, true, this);
		TraceParams.bTraceAsyncScene = true;

		// Perform the trace  
		UWorld* const World = GetWorld();
		GetWorld()->LineTraceSingle(OutHit, StartTrace, EndTrace, ECC_GameTraceChannel1, TraceParams);
		if (OutHit.GetActor() != NULL && OutHit.GetActor()->IsA(AGPCharacter::StaticClass())) {
			const float damage = 5.0f;
			FPointDamageEvent* DamageEvent = new FPointDamageEvent(damage, OutHit, TraceDirection, nullptr);
			OutHit.GetActor()->TakeDamage(damage, *DamageEvent, GetInstigatorController(), this);
		}

		if (World)
		{
			FVector StartToEnd;
			FActorSpawnParameters SpawnParams;
			FRotator Rotation = MuzzleRotation;
			Rotation.Yaw += 90;
			Rotation.Pitch = MuzzleRotation.Pitch;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			SpawnParams.bNoCollisionFail;

			if (OutHit.bBlockingHit)
			{
				StartToEnd = OutHit.ImpactPoint - StartTrace;
			}
			else
			{
				StartToEnd = EndTrace - StartTrace;
			}

			// IMMA FIRIN MAH LAZOR
			AGPLaserBeam* const Lazor = World->SpawnActor<AGPLaserBeam>(LaserBeamClass, StartTrace + (StartToEnd / 2), Rotation, SpawnParams);
			if (Lazor)
			{
				// Play Sound
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Lazor is not null"));
				Lazor->PlaySoundOnActor(ShotGunSound, 0.2f, 0.5f);
				Lazor->SetScale(StartToEnd.Size() - 10.0f);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Lazor is null"));
			}
		}
	}
}

void AGPCharacter::OnBombLaunch()
{
	// WARNING: This condition -MUST- match that in validate, else the client may be disconnected!
	if (CanFire() && RemoteBombList.Num() < MaxBombs)
	{
		ServerOnBombLaunch();
	}
}

bool AGPCharacter::ServerOnBombLaunch_Validate()
{
	// Only allow the character to fire if they have health.
	return (CanFire() && RemoteBombList.Num() < MaxBombs);
}

void AGPCharacter::ServerOnBombLaunch_Implementation()
{
	// If we have been validated by the server, then we need to broadcast the fire event to all clients.
	if (Role == ROLE_Authority) {
		// TODO: Move this client side and validate?
		FVector CameraLoc;
		FRotator CameraRot;
		GetActorEyesViewPoint(CameraLoc, CameraRot);

		BroadcastOnBombLaunch(CameraLoc, CameraRot);
	}
}

void AGPCharacter::BroadcastOnBombLaunch_Implementation(FVector CameraLoc, FRotator CameraRot)
{
	if (RemoteBombClass != NULL)
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
			AGPRemoteBomb* const RemoteBomb = World->SpawnActor<AGPRemoteBomb>(RemoteBombClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (RemoteBomb)
			{
				// find launch direction
				FVector const LaunchDir = MuzzleRotation.Vector();
				RemoteBomb->InitVelocity(LaunchDir);
				BombPlanted = true;
				RemoteBombList.Add(RemoteBomb);
			}
		}
	}
}

void AGPCharacter::OnBombDetonate()
{
	// WARNING: This condition -MUST- match that in validate, else the client may be disconnected!
	if (CanFire() && BombPlanted)
	{
		ServerOnBombDetonate();
	}
}

bool AGPCharacter::ServerOnBombDetonate_Validate()
{
	// Only allow the character to fire if they have health.
	return (CanFire() && BombPlanted);
}

void AGPCharacter::ServerOnBombDetonate_Implementation()
{
	// If we have been validated by the server, then we need to broadcast the fire event to all clients.
	if (Role == ROLE_Authority) {
		// TODO: Move this client side and validate?
		//FVector CameraLoc;
		//FRotator CameraRot;
		//GetActorEyesViewPoint(CameraLoc, CameraRot);

		BroadcastOnBombDetonate();
	}
}

void AGPCharacter::BroadcastOnBombDetonate_Implementation()
{
	if (RemoteBombClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			AGPRemoteBomb* CurRB = NULL;
			for (int i = 0; i < RemoteBombList.Num(); i++)
			{
                CurRB = RemoteBombList[i];
				// Check make sure our actor exists
				if (!CurRB) continue;
				if (!CurRB->IsValidLowLevel()) continue;
				// Explode it
				CurRB->Explode();
			}
			// Remove all entries from the array
			RemoteBombList.Empty();
			BombPlanted = false;
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

void AGPCharacter::OnFlagPickUp() {
    // Increase number of flags
    FlagsPickedUp++;

    // Print total number of flags
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(FlagsPickedUp).Append(" Flags"));

	// Get controller
	//AController* controller = GetController();

	AGPCharacter::SetPauseState();
}

// Check game state = 1 before setting to 2 and starting the reset timer
void AGPCharacter::SetPauseState()
{
	UWorld* const World = GetWorld();
	if (World == NULL || !World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unable to find game world"));
	}
	AGPGameState* gs = Cast<AGPGameState>(World->GetGameState());
	if (gs == NULL || !gs)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unable to find game state"));
	}
	if (gs->GetState() == 1)
	{
		ServerSetPauseState();
	}
}

bool AGPCharacter::ServerSetPauseState_Validate()
{
	AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
	return (gs->GetState() == 1);
}

void AGPCharacter::ServerSetPauseState_Implementation()
{
	if (Role == ROLE_Authority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Setting pause state"));
		AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
		gs->SetState(2);
		// Start timer to go back to normal state
		GetWorld()->GetTimerManager().SetTimer(this, &AGPCharacter::SetPauseStateOff, 3.0f, false, -1.0f);
	}
}

void AGPCharacter::SetPauseStateOff()
{
	UWorld* const World = GetWorld();
	AGPGameState* gs = Cast<AGPGameState>(World->GetGameState());
	if (gs->GetState() == 2)
	{
		ServerSetPauseStateOff();
	}
}

bool AGPCharacter::ServerSetPauseStateOff_Validate()
{
	AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
	return (gs->GetState() == 2);
}

void AGPCharacter::ServerSetPauseStateOff_Implementation()
{
	if (Role == ROLE_Authority)
	{
		// Do reset
		UWorld * const World = GetWorld();
		AGPGameMode * gm = Cast<AGPGameMode>(World->GetAuthGameMode());
		if (gm == NULL || !gm)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GameMode null"));
		}
		else
		{
			gm->ResetBuildings();
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Setting game state"));
		AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
		gs->SetState(1);
	}
}


void AGPCharacter::Tick(float deltaSeconds)
{
	FVector ActorLocation = GetActorLocation();
	if (GetActorLocation().Z <= -5000)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("We died to falling! Oh noes!"));
		Respawn();
	}
}

void AGPCharacter::NextWeapon()
{
	SelectWeapon((Weapon + 1) % 2);
}

void AGPCharacter::PrevWeapon()
{
	int8 NewWeapon = Weapon - 1;
	if (NewWeapon == -1)
	{
		NewWeapon = 1;
	}
	SelectWeapon(NewWeapon);
}

void AGPCharacter::SelectWeapon(int8 NewWeapon)
{
	ServerSelectWeapon(NewWeapon);
}


bool AGPCharacter::ServerSelectWeapon_Validate(int8 NewWeapon)
{
	return true;
}

void AGPCharacter::ServerSelectWeapon_Implementation(int8 NewWeapon)
{
	if (Role == ROLE_Authority) {
		BroadcastSelectWeapon(NewWeapon);
	}
}

void AGPCharacter::BroadcastSelectWeapon_Implementation(int8 NewWeapon)
{
	Weapon = NewWeapon;
}