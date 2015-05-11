// Fill out your copyright notice in the Description page of Project Settings.
#include "GPProject.h"
#include "GPProjectile.h"
#include "GPRemoteBomb.h"
#include "GPCharacter.h"
#include "GPPlayerController.h"
#include "GPGameState.h"
#include "GPGameMode.h"
#include "UnrealNetwork.h"
#include "GPPlayerState.h"
#include "GPCaptureZone.h"

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

	// Create mesh components (two copies) for the weapon
	WeaponMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("WeaponMesh"));
	WeaponMesh->SetOwnerNoSee(true);

	WeaponMeshFirst = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("WeaponFirstMesh"));
	WeaponMeshFirst->SetOnlyOwnerSee(true);

	// everyone but the owner can see the regular body mesh
	GetMesh()->SetOwnerNoSee(true);

	// Set number of flags picked up to zero.
	FlagsPickedUp = 0;

	static ConstructorHelpers::FObjectFinder<UMaterial> Material1(TEXT("Material'/Game/Materials/Red.Red'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> Material2(TEXT("Material'/Game/Materials/Green.Green'"));
	if (Material1.Object != NULL)
	{
		RedMaterial = (UMaterial*)Material1.Object;
		UE_LOG(LogTemp, Warning, TEXT("RedMaterial has a value"));
	}
	if (Material2.Object != NULL)
	{
		GreenMaterial = (UMaterial*)Material2.Object;
		UE_LOG(LogTemp, Warning, TEXT("GreenMaterial has a value"));
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> GunShotSoundCueLoader(TEXT("SoundCue'/Game/Audio/GunShot_Cue.GunShot_Cue'"));
	ShotGunSound = GunShotSoundCueLoader.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> RespawnSoundCueLoader(TEXT("SoundCue'/Game/Audio/Respawn_Cue.Respawn_Cue'"));
	RespawnSound = RespawnSoundCueLoader.Object;
    
    SpawnPoints[0] = FVector( 2300.0f,  3800.0f, 112.0f);
	SpawnPoints[1] = FVector(-2300.0f, -3800.0f, 112.0f);

	static ConstructorHelpers::FObjectFinder<USoundCue> PickUpSoundCueLoader(TEXT("SoundCue'/Game/Audio/PickUp_Cue.PickUp_Cue'"));
	PickUpSound = PickUpSoundCueLoader.Object;
}

bool AGPCharacter::CanJoinTeam(int8 Team)
{
	return true;
}

void AGPCharacter::SetMaterial(int8 Team)
{
	// WARNING: This condition -MUST- match that in validate, else the client may be disconnected!
	if (CanJoinTeam(Team))
	{
		ServerSetMaterial(Team);
	}
}

bool AGPCharacter::ServerSetMaterial_Validate(int8 Team)
{
	return CanJoinTeam(Team);
}

void AGPCharacter::ServerSetMaterial_Implementation(int8 Team)
{
	// If we have been validated by the server, then we need to broadcast the change team to all clients.
	if (Role == ROLE_Authority) {
		BroadcastSetMaterial(Team);
	}
}

void AGPCharacter::BroadcastSetMaterial_Implementation(int8 Team)
{
	if (Role == ROLE_Authority && PlayerState == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server, state is null"));
	}
	else if (Role != ROLE_Authority && PlayerState == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("server, state is null"));
	}
	// Set material on flags for clients
	// Would set them on being play but the flags don't seem to have been spawned for the client by then
	for (TActorIterator<AGPFlagPickup> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->ClientOnlySetMaterial();
	}
	for (TActorIterator<AGPCaptureZone> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->ClientOnlySetColor();
	}
	if (Team == 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Adding green material to player"));
		GetMesh()->SetMaterial(1, UMaterialInstanceDynamic::Create(GreenMaterial, this));
		WeaponMeshFirst->SetMaterial(0, UMaterialInstanceDynamic::Create(GreenMaterial, this));
		WeaponMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(GreenMaterial, this));
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Adding Red Material to player"));
		GetMesh()->SetMaterial(1, UMaterialInstanceDynamic::Create(RedMaterial, this));
		WeaponMeshFirst->SetMaterial(0, UMaterialInstanceDynamic::Create(RedMaterial, this));
		WeaponMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(RedMaterial, this));
	}
}


void AGPCharacter::JoinTeam(int8 Team)
{
	// WARNING: This condition -MUST- match that in validate, else the client may be disconnected!
	if (CanJoinTeam(Team))
	{
		ServerJoinTeam(Team);
	}

}

bool AGPCharacter::ServerJoinTeam_Validate(int8 Team)
{
	return CanJoinTeam(Team);
}

void AGPCharacter::ServerJoinTeam_Implementation(int8 Team)
{
	// If we have been validated by the server, then we need to broadcast the change team to all clients.
	if (Role == ROLE_Authority) {
		BroadcastJoinTeam(Team);
	}
}

void AGPCharacter::BroadcastJoinTeam_Implementation(int8 Team)
{
	if (GetController() != NULL)
	{
		AGPPlayerController* Controller = Cast<AGPPlayerController>(GetController());
		AGPPlayerState* State = Cast<AGPPlayerState>(Controller->PlayerState);
		State->Team = Team;
        Spawn();
	}
}

// Moved this into a server call to prevent dying at 5 health on clients.
// As what happens is the server sets it to 5 health (which replicates to all the client),
// and then the firer calls TakeDamage again and locally sets it to 0
/*float AGPCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// TODO: Implement this properly ourselves (with damage type handlers!)
	// For now, simply call the super method to do anything that might be necessary, and ignore any checks.

	//&& ((AGPPlayerState*)((AGPPlayerController*)EventInstigator)->PlayerState)->Team != ((AGPPlayerState*)PlayerState)->Team

	//((AGPPlayerState*)PlayerState)->Team;
	//((AGPPlayerState*)((AGPPlayerController*)EventInstigator)->PlayerState)->Team;

	UE_LOG(LogTemp, Warning, TEXT("Oh no! We've been hit! What a shame."));
	if (PlayerState != NULL && EventInstigator->PlayerState != NULL)
	{
		int32 OurTeam = ((AGPPlayerState*)PlayerState)->Team;
		UE_LOG(LogTemp, Warning, TEXT("WE ARE %d"), OurTeam);

		int32 TheirTeam = ((AGPPlayerState*)(EventInstigator->PlayerState))->Team;
		if (TheirTeam != OurTeam) {

			Health -= DamageAmount;

			AGPCharacter* otherPlayer = Cast<AGPCharacter, AActor>(DamageCauser->GetOwner());
			otherPlayer->IncreasePoints();

			if (Health <= 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("We died! Oh noes!"));
				Respawn();
			}

			return DamageAmount;
			//return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		}
		else {
			return 0.0f;
		}
	}
	return 0.0f;
}*/

bool AGPCharacter::ServerTakeDamage_Validate(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return true;
}

void AGPCharacter::ServerTakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Role == ROLE_Authority)
	{
		// TODO: Implement this properly ourselves (with damage type handlers!)
		// For now, simply call the super method to do anything that might be necessary, and ignore any checks.

		//&& ((AGPPlayerState*)((AGPPlayerController*)EventInstigator)->PlayerState)->Team != ((AGPPlayerState*)PlayerState)->Team

		//((AGPPlayerState*)PlayerState)->Team;
		//((AGPPlayerState*)((AGPPlayerController*)EventInstigator)->PlayerState)->Team;

		UE_LOG(LogTemp, Warning, TEXT("Oh no! We've been hit! What a shame."));
		if (PlayerState != NULL && EventInstigator->PlayerState != NULL)
		{
			int32 OurTeam = ((AGPPlayerState*)PlayerState)->Team;
			UE_LOG(LogTemp, Warning, TEXT("WE ARE %d"), OurTeam);

			int32 TheirTeam = ((AGPPlayerState*)(EventInstigator->PlayerState))->Team;
			if (TheirTeam != OurTeam) {
				// Only take damage if we still have health, so we don't attempt to respawn while respawning
				if (Health > 0)
				{

					Health -= DamageAmount;

					AGPCharacter* otherPlayer = Cast<AGPCharacter, AActor>(DamageCauser->GetOwner());
					otherPlayer->IncreasePoints();

					if (Health <= 0)
					{
                        UE_LOG(LogTemp, Warning, TEXT("We died! Oh noes!"));
						ServerRespawn(false);
					}
				}
				//return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
			}
		}
	}
}

void AGPCharacter::IncreasePoints() {
	Point += 10.0f;
}

void AGPCharacter::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("A player has entered the game!"));

	// Set starting health

	Health = 100.0f;
	Point = 0.0f;
	BombPlanted = false;
	MaxBombs = 5;
    Ammo = 100;
}

void AGPCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (Role == ROLE_Authority)
	{
	}
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

bool AGPCharacter::ServerSetLightIntensity_Validate(float val)
{
	return true;
}

void AGPCharacter::ServerSetLightIntensity_Implementation(float val)
{
	if (Role == ROLE_Authority)
	{
		BroadcastSetLightIntensity(val);
	}
}

void AGPCharacter::BroadcastSetLightIntensity_Implementation(float val)
{
	TArray<UActorComponent*> components;
	GetComponents(components);
	for (int32 i = 0; i < components.Num(); i++)
	{
		// Find the spotlight component
		UActorComponent* comp = components[i];
		if (components[i]->GetName() == "FlagLight")
		{
			// Set the intensity on all clients so everyone can see we have a flag
			USpotLightComponent * spotlight = Cast<USpotLightComponent>(comp);
			if (spotlight) {
				// Set to 0 for the owner so we don't blind the player
				if (GetController() != NULL && Role != ROLE_Authority)
				{
					spotlight->SetIntensity(0);
				}
				else
				{
					spotlight->SetIntensity(val);
				}
			}
		}
	}
}

// Used when first spawning the actor so there's no delay
void AGPCharacter::Spawn()
{
	// Play Sound
	this->PlaySoundOnActor(RespawnSound, 1.0f, 3.0f);

	AGPPlayerState *pstate = Cast<AGPPlayerState>(PlayerState);
	if (pstate != NULL) {
		int8 Team = pstate->Team;
		SetActorLocationAndRotation(SpawnPoints[Team], FRotator::ZeroRotator, false);
		Health = 100;
		Ammo = 100;
	}
}

bool AGPCharacter::ServerRespawn_Validate(bool shallResetFlag)
{
	return true;
}

void AGPCharacter::ServerRespawn_Implementation(bool shallResetFlag)
{
	if (Role == ROLE_Authority)
	{
		// Play Sound
		resetFlag = shallResetFlag;
		this->PlaySoundOnActor(RespawnSound, 1.0f, 3.0f);
		AGPPlayerState * PState = (AGPPlayerState*)PlayerState;
		int8 Team = Cast<AGPPlayerState>(PlayerState)->Team;
		bool hadFlag = PState->GetHasFlag();
		ServerSetLightIntensity(0.0f);
		FVector loc = GetActorLocation();
		deathLoc = GetActorLocation();
		if (hadFlag)
		{
			BroadcastRespawn();
		}
		BroadcastSetAmmo(100);
		// Broadcast the timer so that it's a valid timer on all clients
		BroadcastRespawnTimer();
		//respawnTimer = FTimerHandle();
		GetWorld()->GetTimerManager().SetTimer(respawnTimer, this, &AGPCharacter::ServerFinishRespawn, 3.0f);
		loc.Z = -500.f;
		SetActorLocationAndRotation(loc, FRotator::ZeroRotator, false);
		if (hadFlag)
		{
			FTimerHandle handle = FTimerHandle();
			GetWorld()->GetTimerManager().SetTimer(handle, this, &AGPCharacter::ServerRespawnDropFlag, 0.2f);
		}
	}
}

void AGPCharacter::BroadcastSetAmmo_Implementation(int32 val)
{
	Ammo = val;
}

void AGPCharacter::BroadcastRespawnTimer_Implementation()
{
	// Set the timer to not actually do anything for clients so that we don't call ServerFinishRespawn half a dozen times
	GetWorld()->GetTimerManager().SetTimer(respawnTimer, 3.0f, false, -1.0f);
}

bool AGPCharacter::ServerRespawnDropFlag_Validate()
{
	return true;
}

void AGPCharacter::ServerRespawnDropFlag_Implementation()
{
	if (Role == ROLE_Authority)
	{
		AGPPlayerState * PState = (AGPPlayerState*)PlayerState;
		int8 Team = Cast<AGPPlayerState>(PlayerState)->Team;
		// Respawn the flag back at capture zone
		if (resetFlag == true)
		{
			if (Team == 0)
			{
				ServerSpawnFlag(SpawnPoints[1], Team, false);
			}
			else
			{
				ServerSpawnFlag(SpawnPoints[0], Team, true);
			}
			resetFlag = false;
		}
		// Drop the flag at our feet
		else
		{
			FVector loc = deathLoc;
			loc.Z = 10.f;
			ServerSpawnFlag(loc, Team, true);
		}
	}
}

// Set states so that we don't instantly repickup the flag
void AGPCharacter::BroadcastRespawn_Implementation()
{
	if (GetController() != NULL)
	{
		AGPPlayerController* Controller = Cast<AGPPlayerController>(GetController());
		AGPPlayerState* State = Cast<AGPPlayerState>(Controller->PlayerState);
		if (State == NULL)
		{
            UE_LOG(LogTemp, Warning, TEXT("State is null"));
		}
		else 
		{
			State->SetHadFlag(true);
			State->SetHasFlag(false);
			State->SetCanPickupFlag(false);
		}
	}
}

bool AGPCharacter::ServerFinishRespawn_Validate()
{
	return true;
}

// Reset player location after delay (so we don't instantly respawn)
void AGPCharacter::ServerFinishRespawn_Implementation()
{
	if (Role == ROLE_Authority)
	{
		AGPPlayerState * PState = (AGPPlayerState*)PlayerState;
		int8 Team = Cast<AGPPlayerState>(PlayerState)->Team;
		Health = 100;
		SetActorLocationAndRotation(SpawnPoints[Team], FRotator::ZeroRotator, false);
		FTimerHandle handle = FTimerHandle();
		GetWorld()->GetTimerManager().SetTimer(handle, this, &AGPCharacter::BroadcastFinishRespawn, 1.0f);
	}
}

// Reset states after player has been moved back to spawn on ALL clients
void AGPCharacter::BroadcastFinishRespawn_Implementation()
{
	if (GetController() != NULL)
	{
		AGPPlayerController* Controller = Cast<AGPPlayerController>(GetController());
		AGPPlayerState* State = Cast<AGPPlayerState>(Controller->PlayerState);
		if (State == NULL)
		{
            UE_LOG(LogTemp, Warning, TEXT("State is null"));
		}
		else {
			State->SetCanPickupFlag(true);
			State->SetHadFlag(false);
		}
	}
}

bool AGPCharacter::getRespawnTimerExists()
{
	return (GetWorld()->GetTimerManager().TimerExists(respawnTimer));
}

float AGPCharacter::getRespawnTimeRemaining()
{
	if (GetWorld()->GetTimerManager().TimerExists(respawnTimer))
	{
		float time = GetWorld()->GetTimerManager().GetTimerRemaining(respawnTimer);
		return time;
	}
	return 0.0f;
}

//void AGPCharacter::MoveForward(float Value)
//{
//  // TODO: Health test - forward disabled when health gone
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
	AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
	return (Health > 0.0f && gs->GetState() == 1 && Ammo > 0 && !gs->GetWaitingForRescan());
}

bool AGPCharacter::CanDetonate()
{
    AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
	return (Health > 0.0f && gs->GetState() == 1 && !gs->GetWaitingForRescan());
}

bool AGPCharacter::CanPlaceBomb()
{
    AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
	return (Health > 0.0f && gs->GetState() == 1 && Ammo > 5 && RemoteBombList.Num() < MaxBombs && !gs->GetWaitingForRescan());
}

void AGPCharacter::OnFire()
{
	// WARNING: This condition -MUST- match that in validate, else the client may be disconnected!
	if (CanFire())
	{
        Ammo--;
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
			FVector const LaunchDir = MuzzleRotation.Vector();
			/*float newY = (1 / sqrt(1 + ((LaunchDir.X*LaunchDir.X) / (LaunchDir.Y*LaunchDir.Y))));
			if (LaunchDir.Y < 0)
			{
				newY = newY * (-1);
			}
			float newX = sqrt(1 - newY*newY);
			if (LaunchDir.X < 0)
			{
				newX = newX * (-1);
			}
			FVector const TwoDLaunchDir = FVector(newX, newY, 0.f);
			FVector const MuzzleLocation2 = MuzzleLocation + TwoDLaunchDir * 100;*/
			AGPProjectile* const Projectile = World->SpawnActor<AGPProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Play Sound
				Projectile->PlaySoundOnActor(ShotGunSound, 0.2f, 0.5f);
				GetCapsuleComponent()->MoveIgnoreActors.Add(Projectile);
				Projectile->InitVelocity(LaunchDir);
			}
		}
	}
}

void AGPCharacter::OnBombLaunch()
{
	// WARNING: This condition -MUST- match that in validate, else the client may be disconnected!
	if (CanPlaceBomb())
	{
        Ammo -= 5;
		ServerOnBombLaunch();
	}
}

bool AGPCharacter::ServerOnBombLaunch_Validate()
{
	// Only allow the character to fire if they have health.
	return (CanPlaceBomb());
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
				RemoteBomb->SetActorRotation(FRotator(FMath::RandRange(0, 360), FMath::RandRange(0, 360), FMath::RandRange(0,360)));
			}
		}
	}
}

void AGPCharacter::OnBombDetonate()
{
	// WARNING: This condition -MUST- match that in validate, else the client may be disconnected!
	if (BombPlanted && CanDetonate())
	{
		ServerOnBombDetonate();
	}
}

bool AGPCharacter::ServerOnBombDetonate_Validate()
{
	// Only allow the character to fire if they have health.
	return (CanDetonate() && BombPlanted);
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

void AGPCharacter::BroadcastRemoveBombs_Implementation()
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
				// Remove it
				CurRB->Destroy();
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

bool AGPCharacter::CanPickupFlag()
{
	// Make sure we're actually looking at the right player before sending through
	if ((AGPPlayerState*)PlayerState != NULL) {
		return (!(((AGPPlayerState*)PlayerState)->GetHasFlag()) && ((AGPPlayerState*)PlayerState)->GetCanPickupFlag());
	}
	else
	{
		return false;
	}
}

bool AGPCharacter::CanCaptureFlag()
{
	if ((AGPPlayerState*)PlayerState != NULL)
	{
		return (((AGPPlayerState*)PlayerState)->GetHasFlag());
	}
	else
	{
		return false;
	}
}

// Defer to server
void AGPCharacter::OnFlagPickup(AGPFlagPickup * flag) {
	if (CanPickupFlag() && GetController() != NULL && Role == ROLE_Authority)
	{
		ServerOnFlagPickup(flag);
	}
}

bool AGPCharacter::ServerOnFlagPickup_Validate(AGPFlagPickup * flag)
{
	return (CanPickupFlag());
}

void AGPCharacter::ServerOnFlagPickup_Implementation(AGPFlagPickup * flag)
{
	if (Role == ROLE_Authority)
	{
		// Tell all that a flag has been picked up
		FVector loc = flag->GetActorLocation();
		BroadcastOnFlagPickup(loc);
		int8 Team = flag->flagTeam;
		GetWorld()->DestroyActor(flag, true);
        // Increase number of flags captured.
        AGPGameState* const MyGameState = GetWorld() != NULL ? GetWorld()->GetGameState<AGPGameState>() : NULL;
        MyGameState->flagsCaptured += 1;
	}
}

void AGPCharacter::BroadcastOnFlagPickup_Implementation(FVector loc)
{
	// If we're that player, change our playerstate (which will replicate automatically)
	if (GetController() != NULL)
	{
		PlaySoundAtLocation(PickUpSound, loc, 0.5f, 0.5f);
		AGPPlayerController* Controller = Cast<AGPPlayerController>(GetController());
		AGPPlayerState* State = Cast<AGPPlayerState>(Controller->PlayerState);
		if (State == NULL)
		{
            UE_LOG(LogTemp, Warning, TEXT("State is null"));
		}
		else {
			State->SetHasFlag(true);
		}
	}
	ServerSetLightIntensity(100000.0f);
}

void AGPCharacter::OnFlagCapture()
{
	AGPPlayerState* PState = (AGPPlayerState*)PlayerState;
	int8 T = PState->Team;
	if (CanCaptureFlag() && GetController() != NULL && Role == ROLE_Authority)
	{
		ServerOnFlagCapture(T);
	}
}

bool AGPCharacter::ServerOnFlagCapture_Validate(int8 Team)
{
	return (CanCaptureFlag());
}

void AGPCharacter::ServerOnFlagCapture_Implementation(int8 Team)
{
	if (Role == ROLE_Authority)
	{
		// Tell everyone a flag has been captured
		BroadcastOnFlagCapture();
		UWorld* const World = GetWorld();
		if (World)
		{
			AGPGameState* gs = Cast<AGPGameState>(World->GetGameState());
			gs->UpdateFlagLeader();
			/*
			// Spawn new flag
			FActorSpawnParameters SpawnParams = FActorSpawnParameters();
			if (this)
			{
				SpawnParams.Owner = this;
			}
			else {
				SpawnParams.Owner = NULL;
			}
			SpawnParams.Instigator = NULL;

			FRotator rotation = FRotator(0.f, 0.f, 0.f);
			FVector location;
			if (Team == 0) {
				location = FVector(-2300.f, -3800.f, 0.f);
			}
			else
			{
				location = FVector(2300.f, 3800.f, 0.f);
			}
			ServerSpawnFlag(location, Team, false);

			AGPFlagPickup* flag = World->SpawnActor<AGPFlagPickup>(AGPFlagPickup::StaticClass(), location, rotation, SpawnParams);

			if (flag == NULL)
			{
				UE_LOG(LogTemp, Warning, TEXT("Flag is null"));
			}
			else {
				flag->Init(Team, false);
				UE_LOG(LogTemp, Warning, TEXT("Flag spawned"));
			}*/
		}
		// Update the current top score
		FVector loc;
		if (Team == 0) {
			loc = FVector(-2300.f, -3800.f, 0.f);
		}
		else
		{
			loc = FVector(2300.f, 3800.f, 0.f);
		}

		ServerSpawnFlag(loc, Team, false);
		// Then pause the game
		SetPauseState();
        // Decrease number of flags captured.
        AGPGameState* const MyGameState = GetWorld() != NULL ? GetWorld()->GetGameState<AGPGameState>() : NULL;
        MyGameState->flagsCaptured -= 1;
	}
}

void AGPCharacter::BroadcastOnFlagCapture_Implementation()
{
	// If we're that player, change our playerstate (which will replicate automatically)
	if (GetController() != NULL)
	{
		FlagsPickedUp++;
		AGPPlayerController* Controller = Cast<AGPPlayerController>(GetController());
		AGPPlayerState* State = Cast<AGPPlayerState>(Controller->PlayerState);
		if (State == NULL)
		{
            UE_LOG(LogTemp, Warning, TEXT("State is null"));
		}
		// Remove flag state and increment our count
		else {
			State->SetHasFlag(false);
			State->IncrementFlags();
		}
	}
	// Get all components of the actor
	ServerSetLightIntensity(0.0f);/*
	TArray<UActorComponent*> components;
	GetComponents(components);
	for (int32 i = 0; i < components.Num(); i++)
	{
		// Find the spotlight component
		UActorComponent* comp = components[i];
		if (components[i]->GetName() == "FlagLight")
		{
			// Set the intensity on all clients so everyone can see we no longer have a flag
			USpotLightComponent * spotlight = Cast<USpotLightComponent>(comp);
			if (spotlight) {
				spotlight->SetIntensity(0.0f);
			}
		}
	}*/
}

void AGPCharacter::OnHealthPickUp() {
    Health = 100.0f;
}

// Check game state = 1 before setting to 2 and starting the reset timer
void AGPCharacter::SetPauseState()
{
	UWorld* const World = GetWorld();
	if (World == NULL || !World)
	{
        UE_LOG(LogTemp, Warning, TEXT("Unable to find game world"));
	}
	AGPGameState* gs = Cast<AGPGameState>(World->GetGameState());
	if (gs == NULL || !gs)
	{
        UE_LOG(LogTemp, Warning, TEXT("Unable to find game state"));
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
        UE_LOG(LogTemp, Warning, TEXT("Setting pause state"));
		AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
		gs->SetState(2);
		// Start timer to go back to normal state TODO: We may want a timeout if the Kinect isn't working?
		gs->SetWaitingForRescan(true);
		for (TActorIterator<AGPCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// We need to tell every actor to do it, so that it is set for all characters on all clients.
			ActorItr->BroadcastRescanTimer();
		}
		BroadcastRescanTimer();
		GetWorld()->GetTimerManager().SetTimer(rescanTimer, this, &AGPCharacter::SetPauseStateOff, 30.0f);
	}
}

void AGPCharacter::BroadcastRescanTimer_Implementation()
{
	// Set it for the clients player, rather than every player on every client
	if (GetController() != NULL)
	{
		// Set the timer to not actually do anything for clients
		GetWorld()->GetTimerManager().SetTimer(rescanTimer, 30.0f, false, -1.0f);
	}
}

bool AGPCharacter::getRescanTimerExists()
{
	return (GetWorld()->GetTimerManager().TimerExists(rescanTimer));
}

float AGPCharacter::getRescanTimeRemaining()
{
	if (GetWorld()->GetTimerManager().TimerExists(rescanTimer))
	{
		float time = GetWorld()->GetTimerManager().GetTimerRemaining(rescanTimer);
		return time;
	}
	return 0.0f;
}

// TODO: Rename
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
            UE_LOG(LogTemp, Warning, TEXT("GameMode null"));
		}
		else
		{
			gm->ResetBombs();
			gm->ResetBuildings();
			gm->Rescan();
		}
		/*UE_LOG(LogTemp, Warning, TEXT("Setting game state"));
		AGPGameState* gs = Cast<AGPGameState>(GetWorld()->GetGameState());
		gs->SetState(1);*/
	}
}

bool AGPCharacter::ServerSpawnFlag_Validate(FVector loc, int8 Team, bool wasDropped) {
	return true;
}

void AGPCharacter::ServerSpawnFlag_Implementation(FVector loc, int8 Team, bool wasDropped) {
	if (Role == ROLE_Authority) 
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			AGPGameState* gs = Cast<AGPGameState>(World->GetGameState());
			gs->UpdateFlagLeader();

			// Spawn new flag
			FActorSpawnParameters SpawnParams = FActorSpawnParameters();
			if (this)
			{
				SpawnParams.Owner = this;
			}
			else {
				SpawnParams.Owner = NULL;
			}
			SpawnParams.Instigator = NULL;
			SpawnParams.bNoFail = true;

			FRotator rotation = FRotator(0.f, 0.f, 0.f);
			AGPGameMode* gm = Cast<AGPGameMode>(World->GetAuthGameMode());
			AGPFlagPickup* flag = NULL;
			if (gm != NULL)
			{
				if (gm->FlagPickupBPClass != NULL)
				{
					flag = World->SpawnActor<AGPFlagPickup>(gm->FlagPickupBPClass, loc, rotation, SpawnParams);
				}
				else
				{
					flag = World->SpawnActor<AGPFlagPickup>(AGPFlagPickup::StaticClass(), loc, rotation, SpawnParams);
				}
			}
			else
			{
				flag = World->SpawnActor<AGPFlagPickup>(AGPFlagPickup::StaticClass(), loc, rotation, SpawnParams);
			}
			if (flag == NULL)
			{
                UE_LOG(LogTemp, Warning, TEXT("Flag is null"));
			}
			else {
				flag->Init(Team, wasDropped);
                UE_LOG(LogTemp, Log, TEXT("Flag spawned"));
			}
		}
	}
}

void AGPCharacter::Tick(float deltaSeconds)
{
	FVector ActorLocation = GetActorLocation();
	if (GetActorLocation().Z <= -100000 && Health > 0)
	{
        UE_LOG(LogTemp, Warning, TEXT("We died to falling! Oh noes!"));
		// Move flag back to capture area
		ServerRespawn(true);
	}
}

float AGPCharacter::getHealth()
{
    return (float)Health;
}

int32 AGPCharacter::getAmmo()
{
    return Ammo;
}

void AGPCharacter::OnAmmoPickUp(int32 Value) {
    Ammo += Value;
}
