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

    GetCharacterMovement()->MaxWalkSpeed = 280.0f;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->GravityScale = 1.5f;
    GetCharacterMovement()->JumpZVelocity = 620;
    GetCharacterMovement()->MaxWalkSpeedCrouched = 200;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->AirControl = 0.2f;

    GetMesh()->SetOwnerNoSee(false);
    GetMesh()->SetIsReplicated(true);

    // set our turn rates for input
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

    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    //Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    //Mesh1P->SetOnlyOwnerSee(true);
    //Mesh1P->SetupAttachment(OverShoulderCamera);
    //Mesh1P->bCastDynamicShadow = false;
    //Mesh1P->CastShadow = false;
    //Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
    //Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

    FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    FP_MuzzleLocation->SetupAttachment(TP_Gun);
    FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

    // Default offset from the character location for projectiles to spawn
    GunOffset = FVector(100.0f, 0.0f, 10.0f);

    // Create a gun mesh component
    TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
    //TP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
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
    GetCharacterMovement()->SetIsReplicated(true);
    GetCharacterMovement()->SetNetAddressable();
}

void ACaptureTheFlagCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Retrive the health component.

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

    if (IsPlayerControlled())
    {
        DetachFromControllerPendingDestroy();
    }
}

void ACaptureTheFlagCharacter::NMC_OnClientDeath_Implementation()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    UE_LOG(LogTemp, Warning, TEXT("DEAD"));

    // Simulate character's ragdoll.
    SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SkeletalMesh->SetSimulatePhysics(true);
}

void ACaptureTheFlagCharacter::NMC_PlayWeaponFiringAnimation_Implementation()
{
    AnimationInstance->Montage_Play(FireAnimation);

    // try and play the sound if specified
    if (FireSound != NULL)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }

    // try and play a firing animation if specified
    //if (FireAnimation != NULL)
    //{
    //    // Get the animation object for the arms mesh
    //    UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
    //    if (AnimInstance != NULL)
    //    {
    //        AnimInstance->Montage_Play(FireAnimation, 1.f);
    //    }
    //}

    // try and play a firing animation if specified
    //if (FireAnimationTP != NULL)
    //{
    //    // Get the animation object for the arms mesh
    //    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    //    if (AnimInstance != NULL)
    //    {
    //        AnimInstance->Montage_Play(FireAnimationTP, 1.f);
    //    }
    //}
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

            //Set Spawn Collision Handling Override
            FActorSpawnParameters ActorSpawnParams;
            ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

            // spawn the projectile at the muzzle
            //World->SpawnActor<ACaptureTheFlagProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

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

void ACaptureTheFlagCharacter::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        // add movement in that direction
        AddMovementInput(GetActorRightVector(), Value);
    }
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

//UHealthComponent* ACaptureTheFlagCharacter::GetHealth()
//{
//    return Health;
//}

void ACaptureTheFlagCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACaptureTheFlagCharacter, FireRate);
}