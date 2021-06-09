// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagCharacter.h"
#include "../CaptureTheFlag.h"
#include "../CaptureTheFlagProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterBaseAnimation.h"
#include <GameFramework/PlayerController.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "../Health/HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"
#include "../GameMode/CaptureTheFlagGameMode.h"
#include "../GameState/CaptureTheFlagGameState.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ACaptureTheFlagCharacter

ACaptureTheFlagCharacter::ACaptureTheFlagCharacter() :
    Health(nullptr),
    SkeletalMesh(nullptr),
    AnimationInstance(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->GravityScale = 1.5f;
    GetCharacterMovement()->JumpZVelocity = 620;
    GetCharacterMovement()->MaxWalkSpeedCrouched = 200;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->AirControl = 0.2f;
    GetCharacterMovement()->SetIsReplicated(true);
    GetCharacterMovement()->SetNetAddressable();

    //GetMesh()->SetIsReplicated(true);

    // Set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    OverShoulderCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ShoulderCameraBoom"));
    OverShoulderCameraBoom->SetupAttachment(GetCapsuleComponent());
    OverShoulderCameraBoom->bUsePawnControlRotation = false; // Fixed Camera
    OverShoulderCameraBoom->TargetArmLength = 75.0f;

    // Create a CameraComponent	
    OverShoulderCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ShoulderCamera"));
    OverShoulderCamera->SetupAttachment(OverShoulderCameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    OverShoulderCamera->bUsePawnControlRotation = false;

    FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    FP_MuzzleLocation->SetupAttachment(TP_Gun);
    FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

    // Default offset from the character location for projectiles to spawn
    GunOffset = FVector(100.0f, 0.0f, 10.0f);

    // Create a gun mesh component
    TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
    TP_Gun->SetOwnerNoSee(false);
    TP_Gun->bCastDynamicShadow = false;
    TP_Gun->CastShadow = false;
    TP_Gun->SetIsReplicated(true);
    TP_Gun->SetupAttachment(RootComponent);

    // Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
    // are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
    Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

    FireRate = 10.0f;

    SetCanBeDamaged(true);

    bReplicates = true;
    SetReplicates(true);
    SetReplicateMovement(true);
}

void ACaptureTheFlagCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Subscribe to health component's events.
    if (Health != nullptr)
    {
        Health->OnDeath.AddDynamic(this, &ACaptureTheFlagCharacter::OnDeath);
    }

    // Retrieve the skeletal mesh component.
    SkeletalMesh = GetMesh();
    if (SkeletalMesh != nullptr)
    {
        // Retrieve the animation instance.
        AnimationInstance = Cast<UCharacterBaseAnimation>(SkeletalMesh->GetAnimInstance());
    }
}

void ACaptureTheFlagCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();

    //Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
    TP_Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));

    if (GetLocalRole() == ROLE_Authority)
    {
        UWorld* World = GetWorld();

        if (World != nullptr)
        {
            ACaptureTheFlagGameState* GS = World->GetGameState<ACaptureTheFlagGameState>();

            if (GS != nullptr)
            {
                //GS->AddToTeam(this);
            }
        }
    }

}

void ACaptureTheFlagCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Set animation movement parameters.
    float CurrentSpeed = GetVelocity().Size();
    bool bIsMoving = CurrentSpeed > 0.0f && GetCharacterMovement()->IsMovingOnGround();

    AnimationInstance->bIsMoving = bIsMoving;
    AnimationInstance->MovementSpeed = bIsMoving ? CurrentSpeed : 0.0f;

    // Set animation strafing rotation paremeter.
    FVector MovementDirection = GetLastMovementInputVector();
    FVector CharacterDirection = GetActorForwardVector();

    if (!MovementDirection.IsNearlyZero())
    {
        float StrafingRotation = FMath::Atan2(MovementDirection.Y, MovementDirection.X) - FMath::Atan2(CharacterDirection.Y, CharacterDirection.X);

        if (FMath::Abs(StrafingRotation) > PI)
        {
            StrafingRotation = StrafingRotation > 0 ? StrafingRotation - PI * 2.0f : StrafingRotation + PI * 2.0f;
        }

        StrafingRotation = FMath::RadiansToDegrees(StrafingRotation);

        AnimationInstance->StrafingRotation = StrafingRotation;
    }
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACaptureTheFlagCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // set up gameplay key bindings
    check(PlayerInputComponent);

    // Bind fire event
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACaptureTheFlagCharacter::OnFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACaptureTheFlagCharacter::StopFire);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACaptureTheFlagCharacter::Server_Crouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACaptureTheFlagCharacter::Server_StopCrouch);

    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACaptureTheFlagCharacter::Aim);
    PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACaptureTheFlagCharacter::StopAim);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACaptureTheFlagCharacter::Server_Run);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACaptureTheFlagCharacter::Server_StopRun);
}

void ACaptureTheFlagCharacter::ApplyDamage(AActor* DamageCauser)
{
    UE_LOG(LogTemp, Warning, TEXT("ACaptureTheFlagCharacter:: DamageCaused"));

    // GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Damage Caused By: " + DamageCauser->GetActorLabel());
    FDamageEvent DamageEvent(UDamageType::StaticClass());
    TakeDamage(10.0f, DamageEvent, nullptr, DamageCauser);
}


void ACaptureTheFlagCharacter::OnDeath(AActor* KilledBy)
{
    // Stop ticking while dead.
    PrimaryActorTick.bCanEverTick = false;

    NMC_OnClientDeath();

    //if (IsPlayerControlled())
    //{
    //    DetachFromControllerPendingDestroy();
    //}

}

void ACaptureTheFlagCharacter::NMC_OnClientDeath_Implementation()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    UE_LOG(LogTemp, Warning, TEXT("DEAD"));

    // Simulate character's ragdoll.
    SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SkeletalMesh->SetSimulatePhysics(true);

    // Start a respawn handler to CALL Respawn() after 5 seconds(Non Looping)
    FTimerHandle respawn;
    GetWorldTimerManager().SetTimer(respawn, this, &ACaptureTheFlagCharacter::Respawn, 5.0f, false);
}

void ACaptureTheFlagCharacter::Respawn()
{
    //if (GetLocalRole() == ROLE_Authority)
    {
        ACaptureTheFlagGameMode* GM = Cast<ACaptureTheFlagGameMode>(GetWorld()->GetAuthGameMode());
        if (GM != nullptr)
        {
            GM->RespawnPlayer(Cast<APlayerController>(GetController()));
            Destroy();
        }
    }
}

void ACaptureTheFlagCharacter::NMC_PlayWeaponFiringAnimation_Implementation()
{
    AnimationInstance->Montage_Play(FireAnimation);

    // try and play the sound if specified
    if (FireSound != NULL)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }
}

bool ACaptureTheFlagCharacter::Server_Fire_Validate()
{
    return true;
}

void ACaptureTheFlagCharacter::Server_Fire_Implementation()
{
    // try and fire a projectile
    if (ProjectileClass != NULL)
    {
        UWorld* const World = GetWorld();
        if (World != NULL)
        {

            const FRotator SpawnRotation = GetControlRotation();
            // MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
            const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

            // Set Spawn Collision Handling Override
            FActorSpawnParameters ActorSpawnParams;
            ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

            // Spawn the projectile at the muzzle
            FVector MuzzleLocation = TP_Gun->GetSocketLocation("Muzzle");

            World->SpawnActor<ACaptureTheFlagProjectile>(ProjectileClass, MuzzleLocation, SpawnRotation, ActorSpawnParams);
        }
    }

    NMC_PlayWeaponFiringAnimation();
}

void ACaptureTheFlagCharacter::OnFire()
{
    auto& TimerManager = GetWorld()->GetTimerManager();

    // Start the firing timer and use the remaining time of the previous timer.
    float RemainingTime = FMath::Max(TimerManager.GetTimerRemaining(FireTimer), 0.0f);
    TimerManager.SetTimer(FireTimer, this, &ACaptureTheFlagCharacter::Server_Fire, 1.0f / FireRate, true, RemainingTime);
}

void ACaptureTheFlagCharacter::StopFire()
{
    auto& TimerManager = GetWorld()->GetTimerManager();

    // Replace timer with one that will prevent the primary fire timer from triggering again too quickly.
    if (TimerManager.TimerExists(FireTimer))
    {
        float RemainingTime = TimerManager.GetTimerRemaining(FireTimer);
        TimerManager.SetTimer(FireTimer, this, &ACaptureTheFlagCharacter::ClearFireTimer, RemainingTime, false);
    }
}

void ACaptureTheFlagCharacter::ClearFireTimer()
{
    // Clear the timer after a delay set in ReleaseTrigger() function.
    GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void ACaptureTheFlagCharacter::MoveForward(float Value)
{
    if (Value != 0.0f)
    {
        // add movement in that direction
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

void ACaptureTheFlagCharacter::Server_MoveForward_Implementation(float Value)
{
    NMC_MoveForward(Value);
}

bool ACaptureTheFlagCharacter::Server_MoveForward_Validate(float Value)
{
    return true;
}

void ACaptureTheFlagCharacter::NMC_MoveForward_Implementation(float Value)
{
    MoveForward(Value);
}

void ACaptureTheFlagCharacter::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        // add movement in that direction
        AddMovementInput(GetActorRightVector(), Value);
    }
}

void ACaptureTheFlagCharacter::Server_MoveRight_Implementation(float Value)
{
    NMC_MoveRight(Value);
}

bool ACaptureTheFlagCharacter::Server_MoveRight_Validate(float Value)
{
    return true;
}

void ACaptureTheFlagCharacter::NMC_MoveRight_Implementation(float Value)
{
    MoveRight(Value);
}

void ACaptureTheFlagCharacter::TurnAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACaptureTheFlagCharacter::LookUpAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACaptureTheFlagCharacter::Crouching()
{
    bIsCrouching = true;
    AnimationInstance->bIsCrouched = bIsCrouching;
}

void ACaptureTheFlagCharacter::StopCrouch()
{
    bIsCrouching = false;
    AnimationInstance->bIsCrouched = bIsCrouching;
}

void ACaptureTheFlagCharacter::NMC_Crouch_Implementation()
{
    Crouching();
}

bool ACaptureTheFlagCharacter::Server_Crouch_Validate()
{
    return true;
}

void ACaptureTheFlagCharacter::Server_Crouch_Implementation()
{
    // if (GetLocalRole() == ROLE_Authority)
    {
        NMC_Crouch();
    }
}

void ACaptureTheFlagCharacter::NMC_StopCrouch_Implementation()
{
    StopCrouch();
}

bool ACaptureTheFlagCharacter::Server_StopCrouch_Validate()
{
    return true;
}

void ACaptureTheFlagCharacter::Server_StopCrouch_Implementation()
{
    //if (GetLocalRole() == ROLE_Authority)
    {
        NMC_StopCrouch();
    }
}

void ACaptureTheFlagCharacter::Aim()
{
    bIsAiming = true;
    AnimationInstance->bIsAiming = bIsAiming;
    OverShoulderCamera->SetFieldOfView(60);
}

void ACaptureTheFlagCharacter::StopAim()
{
    bIsAiming = false;
    AnimationInstance->bIsAiming = bIsAiming;
    OverShoulderCamera->SetFieldOfView(90);
}

void ACaptureTheFlagCharacter::Run()
{
    bIsRunning = true;
    bIsCrouching = false;
    AnimationInstance->bIsRunning = bIsRunning;
    AnimationInstance->bIsCrouched = bIsCrouching;
    GetCharacterMovement()->MaxWalkSpeed = 750.0f;
}

void ACaptureTheFlagCharacter::StopRun()
{
    bIsRunning = false;
    AnimationInstance->bIsRunning = bIsRunning;
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

void ACaptureTheFlagCharacter::NMC_Run_Implementation()
{
    Run();
}

bool ACaptureTheFlagCharacter::Server_Run_Validate()
{
    return true;
}

void ACaptureTheFlagCharacter::Server_Run_Implementation()
{
    //if (GetLocalRole() == ROLE_Authority)
    {
        NMC_Run();
    }
}

void ACaptureTheFlagCharacter::NMC_StopRun_Implementation()
{
    StopRun();
}

bool ACaptureTheFlagCharacter::Server_StopRun_Validate()
{
    return true;
}

void ACaptureTheFlagCharacter::Server_StopRun_Implementation()
{
    //if (GetLocalRole() == ROLE_Authority)
    {
        NMC_StopRun();
    }
}

void ACaptureTheFlagCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACaptureTheFlagCharacter, FireRate);
    DOREPLIFETIME(ACaptureTheFlagCharacter, bIsAiming);
    DOREPLIFETIME(ACaptureTheFlagCharacter, bIsRunning);
    DOREPLIFETIME(ACaptureTheFlagCharacter, bIsCrouching);
}