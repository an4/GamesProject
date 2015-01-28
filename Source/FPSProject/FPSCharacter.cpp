// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSProject.h"
#include "FPSProjectile.h"
#include "FPSCharacter.h"

AFPSCharacter::AFPSCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Create a CameraComponent 
    FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->AttachParent = CapsuleComponent;
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
    Mesh->SetOwnerNoSee(true);
}

void AFPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("We are using FPSCharacter!"));
    }
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
    // set up gameplay key bindings
    InputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
    InputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
    InputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

    InputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::OnStartJump);
    InputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::OnStopJump);

    InputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::OnFire);
}

void AFPSCharacter::MoveForward(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // find out which way is forward
        FRotator Rotation = Controller->GetControlRotation();
        // Limit pitch when walking or falling
        if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
        {
            Rotation.Pitch = 0.0f;
        }
        // add movement in that direction
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AFPSCharacter::MoveRight(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        // add movement in that direction
        AddMovementInput(Direction, Value);
    }
}

void AFPSCharacter::OnStartJump()
{
    bPressedJump = true;
}

void AFPSCharacter::OnStopJump()
{
    bPressedJump = false;
}

void AFPSCharacter::OnFire()
{
    // try and fire a projectile
    if (ProjectileClass != NULL)
    {
        // Get the camera transform
        FVector CameraLoc;
        FRotator CameraRot;
        GetActorEyesViewPoint(CameraLoc, CameraRot);
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
            AFPSProjectile* const Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
            if (Projectile)
            {
                // find launch direction
                FVector const LaunchDir = MuzzleRotation.Vector();
                Projectile->InitVelocity(LaunchDir);
            }
        }
    }
}