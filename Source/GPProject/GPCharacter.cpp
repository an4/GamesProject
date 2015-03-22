// Fill out your copyright notice in the Description page of Project Settings.
#include "GPProject.h"
#include "GPProjectile.h"
#include "GPRemoteBomb.h"
#include "GPCharacter.h"
#include "GPPlayerController.h"
#include "GPGameState.h"
#include "GPGameMode.h"
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

	SpawnPoints[0] = FVector(2400.0f, 0.0f, 112.0f);
	SpawnPoints[1] = FVector(-2400.0f, 0.0f, 112.0f);
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
	if (((AGPPlayerState*)PlayerState)->Team == 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Adding green material to player"));
		GetMesh()->SetMaterial(0, UMaterialInstanceDynamic::Create(GreenMaterial, this));
		FirstPersonMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(GreenMaterial, this));
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Adding Red Material to player"));
		GetMesh()->SetMaterial(0, UMaterialInstanceDynamic::Create(RedMaterial, this));
		FirstPersonMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(RedMaterial, this));
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
	}
}

float AGPCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
	return 0.0f;
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

void AGPCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (Role == ROLE_Authority)
	{
		//SetupTeam();
	}
}

void AGPCharacter::SetupTeam()
{
	AGPPlayerState* State = (AGPPlayerState*)PlayerState;
	if (State != NULL)
	{
		SetMaterial(((AGPPlayerState*)PlayerState)->Team);
		UE_LOG(LogTemp, Warning, TEXT("We should be all colourfull"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("We don't have a playerstate :("));
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

void AGPCharacter::Respawn()
{
	// Play Sound
	this->PlaySoundOnActor(RespawnSound, 1.0f, 3.0f);

	int8 Team = Cast<AGPPlayerState>(PlayerState)->Team;
	SetActorLocationAndRotation(SpawnPoints[Team], FRotator::ZeroRotator, false);
	Health = 100;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("We have been respawned!"));
	}
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
				// Play Sound
				Projectile->PlaySoundOnActor(ShotGunSound, 0.2f, 0.5f);

				// find launch direction
				FVector const LaunchDir = MuzzleRotation.Vector();
				Projectile->InitVelocity(LaunchDir);
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
