// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#include "CaptureTheFlagCharacter.h"
#include "../CaptureTheFlag.h"
#include "../CaptureTheFlagProjectile.h"
#include "../GameMode/CaptureTheFlagGameMode.h"
#include "../GameState/CaptureTheFlagGameState.h"
#include "../Health/HealthComponent.h"
#include "../Items/Flag.h"
#include "../PlayerState/CaptureTheFlagPlayerState.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "CharacterBaseAnimation.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../CaptureTheFlagHUD.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

// Sets default values for this character's properties
ACaptureTheFlagCharacter::ACaptureTheFlagCharacter() :
    Health(nullptr),
    SkeletalMesh(nullptr),
    AnimationInstance(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // Character Movement
    {
        GetCharacterMovement()->MaxWalkSpeed = 500.0f;
        GetCharacterMovement()->bOrientRotationToMovement = false;
        GetCharacterMovement()->GravityScale = 1.5f;
        GetCharacterMovement()->JumpZVelocity = 620;
        GetCharacterMovement()->MaxWalkSpeedCrouched = 200;
        GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
        GetCharacterMovement()->AirControl = 0.2f;
        GetCharacterMovement()->SetIsReplicated(true);
        GetCharacterMovement()->SetNetAddressable();
    }

    // Set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // Camera Boom
    {
        OverShoulderCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ShoulderCameraBoom"));
        OverShoulderCameraBoom->SetupAttachment(GetCapsuleComponent());
        OverShoulderCameraBoom->bUsePawnControlRotation = false; // Fixed Camera
        OverShoulderCameraBoom->TargetArmLength = 75.0f;
    }

    // Camera Component
    {
        OverShoulderCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ShoulderCamera"));
        OverShoulderCamera->SetupAttachment(OverShoulderCameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
        OverShoulderCamera->bUsePawnControlRotation = false;
    }

    // Muzzle Location
    {
        FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
        FP_MuzzleLocation->SetupAttachment(TP_Gun);
        FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
    }

    // Default offset from the character location for projectiles to spawn
    GunOffset = FVector(100.0f, 0.0f, 10.0f);

    // Gun Mesh
    {
        TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
        TP_Gun->SetOwnerNoSee(false);
        TP_Gun->bCastDynamicShadow = false;
        TP_Gun->CastShadow = false;
        TP_Gun->SetIsReplicated(true);
        TP_Gun->SetupAttachment(RootComponent);
        FireRate = 10.0f;
    }

    // Health Component
    {
        Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
        SetCanBeDamaged(true);
    }

    bReplicates = true;
    SetReplicates(true);
    SetReplicateMovement(true);

    Flag = nullptr;
}

// Callled after all components have been initialized
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

// Called when the owning Actor begins play
void ACaptureTheFlagCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();

    //Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
    TP_Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));

    PlayerName = GetFName().ToString(); // TEMP

    if (GetLocalRole() > ROLE_AutonomousProxy)
    {
        NetIndex = GetWorld()->GetGameState()->AuthorityGameMode->GetNumPlayers() - 1;
    }

    GetWorldTimerManager().SetTimer(UpdateHandle, this, &ACaptureTheFlagCharacter::UpdateAndCheckPlayer, 0.03333f, true, 0.0f);
    GetWorldTimerManager().SetTimer(PostBeginPlayDelay, this, &ACaptureTheFlagCharacter::Server_PostBeginPlay, 5.0f, false);
}

bool ACaptureTheFlagCharacter::Server_PostBeginPlay_Validate()
{
    return true;
}
void ACaptureTheFlagCharacter::Server_PostBeginPlay_Implementation()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        if (GetCharacterPlayerState())
        {
            Multicast_AssignTeamsColor(GetCharacterPlayerState()->PlayerTeam, NetIndex);
        }
    }
}

void ACaptureTheFlagCharacter::UpdateAndCheckPlayer()
{
    if (GetGameState())
    {
        TeamOneCount = GetGameState()->TeamOneSize;
        TeamTwoCount = GetGameState()->TeamTwoSize;
        TeamOneScore = GetGameState()->TeamOneScore;
        TeamTwoScore = GetGameState()->TeamTwoScore;
    }
}

// Assigns a team to the player that logs in
void ACaptureTheFlagCharacter::AssignTeams()
{
    if (!GetGameState())
        return;

    TeamOneCount = GetGameState()->TeamOneSize;
    TeamTwoCount = GetGameState()->TeamTwoSize;

    if (TeamOneCount == TeamTwoCount)
    {
        //If the Net Mode is Dedicated Server
        if (GetNetMode() == ENetMode::NM_DedicatedServer)
        {
            GetGameState()->TeamOneSize++;
            PlayerTeam = 0;
        }
        else
        {
            GetGameState()->TeamTwoSize++;
            PlayerTeam = 1;
        }
    }

    //If either of the teams is larger than the other, assign the player to the team with less to balance them
    else
    {
        if (TeamOneCount > TeamTwoCount)
        {
            GetGameState()->TeamTwoSize++;
            PlayerTeam = 1;
        }
        else if (TeamOneCount < TeamTwoCount)
        {
            GetGameState()->TeamOneSize++;
            PlayerTeam = 0;
        }
    }

    if (GetCharacterPlayerState() != nullptr)
    {
        GetCharacterPlayerState()->PlayerTeam = PlayerTeam;
    }
}

// Assigns a Network Index to the player that logs in
void ACaptureTheFlagCharacter::AssignNetIndex()
{
    NetIndex = GetGameState()->AuthorityGameMode->GetNumPlayers() - 1;
}

// Multicasts to all clients to assign the team color when player logs in
void ACaptureTheFlagCharacter::Multicast_AssignTeamsColor_Implementation(int team, int index)
{
    if (GetGameState() != nullptr)
    {
        for (auto PS : GetGameState()->PlayerArray)
        {
            ACaptureTheFlagPlayerState* BPS = Cast<ACaptureTheFlagPlayerState>(PS);
            if (BPS != nullptr)
            {
                ACaptureTheFlagCharacter* CB = Cast<ACaptureTheFlagCharacter>(BPS->GetPawn());
                if (CB != nullptr)
                {
                    if (BPS->PlayerTeam == 0)
                    {
                        //If the first person material array for team one isn't null,
                        //assign those materials to the first person mesh
                        if (GetGameState()->TeamOnePMaterials.Num() > 0)
                        {
                            DefaultTPMaterials = GetGameState()->TeamOnePMaterials;
                            CB->SetMaterialToMesh(CB->GetSkeletalMesh(), DefaultTPMaterials);
                        }
                    }
                    else if (BPS->PlayerTeam == 1)
                    {
                        if (GetGameState()->TeamTwoPMaterials.Num() > 0)
                        {
                            DefaultTPMaterials = GetGameState()->TeamTwoPMaterials;
                            CB->SetMaterialToMesh(CB->GetSkeletalMesh(), DefaultTPMaterials);
                        }
                    }
                }
            }
        }
    }
}

// Sets the material to the mesh
void ACaptureTheFlagCharacter::SetMaterialToMesh(USkeletalMeshComponent* InMeshComp, const TArray<UMaterialInterface*>& InMaterials)
{
    if (InMeshComp != nullptr && InMaterials.Num() > 0)
    {
        for (int i = 0; i < InMaterials.Num(); i++)
        {
            UMaterialInterface* MaterialToAssign = InMaterials[i];

            if (MaterialToAssign != nullptr)
            {
                InMeshComp->SetMaterial(i, MaterialToAssign);
            }
        }
    }
}

// Called every frame
void ACaptureTheFlagCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Set animation movement parameters.
    float CurrentSpeed = GetVelocity().Size();
    bool bIsMoving = CurrentSpeed > 0.0f && GetCharacterMovement()->IsMovingOnGround();

    AnimationInstance->bIsMoving = bIsMoving;
    AnimationInstance->MovementSpeed = bIsMoving ? CurrentSpeed : 0.0f;

    Server_AimRotate(DeltaTime);


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

bool ACaptureTheFlagCharacter::Server_AimRotate_Validate(float DeltaTime)
{
    return true;
}

void ACaptureTheFlagCharacter::Server_AimRotate_Implementation(float DeltaTime)
{
    FRotator target = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
    FRotator current = UKismetMathLibrary::MakeRotator(0.0f, AnimationInstance->AimPitch, AnimationInstance->AimYaw);
    FRotator rotation = UKismetMathLibrary::RInterpTo(current, target, DeltaTime, 15.0f);

    float Roll = 0.0f;
    float Pitch = 0.0f;
    float Yaw = 0.0f;

    UKismetMathLibrary::BreakRotator(rotation, Roll, Pitch, Yaw);

    AimPitch = UKismetMathLibrary::ClampAngle(Pitch, -90.0f, 90.0f);
    AimYaw = UKismetMathLibrary::ClampAngle(Yaw, -90.0f, 90.0f);

    AnimationInstance->AimPitch = AimPitch;
    AnimationInstance->AimYaw = AimYaw;
}

// Input
void ACaptureTheFlagCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // set up gameplay key bindings
    check(PlayerInputComponent);

    if (GetCharacterPlayerState())
        GetCharacterPlayerState()->SetPlayerName(PlayerName);
}

// Called when applying damage to player
void ACaptureTheFlagCharacter::ApplyDamage(AActor* DamageCauser)
{
    UE_LOG(LogTemp, Warning, TEXT("ACaptureTheFlagCharacter:: DamageCaused"));

    // GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Damage Caused By: " + DamageCauser->GetActorLabel());
    FDamageEvent DamageEvent(UDamageType::StaticClass());
    TakeDamage(10.0f, DamageEvent, nullptr, DamageCauser);
}

// Called when player dies
void ACaptureTheFlagCharacter::OnDeath(AActor* KilledBy)
{
    // Stop ticking while dead.
    PrimaryActorTick.bCanEverTick = false;

    NMC_OnClientDeath();

    //if (IsPlayerControlled())
    //{
    //    DetachFromControllerPendingDestroy();
    //}

    if (Flag != nullptr)
    {
        Flag->SetState(ECaptureFlagState::InBase, this);
        Flag = nullptr;
    }

    if (GetLocalRole() == ROLE_Authority)
    {
        if (PlayerTeam == 0)
        {
            GetGameState()->TeamOneSize--;
        }
        else
        {
            GetGameState()->TeamTwoSize--;
        }
    }
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

// Called when respawning player
void ACaptureTheFlagCharacter::Respawn()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        ACaptureTheFlagGameMode* GM = Cast<ACaptureTheFlagGameMode>(GetWorld()->GetAuthGameMode());
        if (GM != nullptr)
        {
            GM->RespawnPlayer(Cast<APlayerController>(GetController()), PlayerTeam, NetIndex);
            Destroy();
        }
    }
}

// Called when playing weapon animation
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

// Called when firing weapon
void ACaptureTheFlagCharacter::Server_Fire_Implementation()
{
    // Try and fire a projectile
    if (ProjectileClass != NULL)
    {
        UWorld* const World = GetWorld();
        if (World != NULL)
        {
            const FRotator SpawnRotation = GetControlRotation();
            //const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

            // Set Spawn Collision Handling Override
            FActorSpawnParameters ActorSpawnParams;
            ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
            ActorSpawnParams.Owner = this;

            // Spawn the projectile at the muzzle
            FVector MuzzleLocation = TP_Gun->GetSocketLocation("Muzzle");

            FVector viewDir = OverShoulderCamera->GetComponentRotation().Vector();
            viewDir.Normalize();

            FVector traceStart = MuzzleLocation;
            FVector traceEnd = traceStart + viewDir * 5000.0f;

            FCollisionQueryParams queryParams;
            queryParams.bTraceComplex = true;
            queryParams.AddIgnoredActor(this);
            queryParams.bReturnPhysicalMaterial = false;

            FHitResult Hit;
            FVector projectileDirection;

            if (World->LineTraceSingleByChannel(Hit, traceStart, traceEnd, ECC_Visibility, queryParams) == true)
            {
                projectileDirection = traceStart + viewDir;
            }

            World->SpawnActor<ACaptureTheFlagProjectile>(ProjectileClass, projectileDirection, SpawnRotation, ActorSpawnParams);
        }
        NMC_PlayWeaponFiringAnimation();
    }
}

// Fires a projectile
void ACaptureTheFlagCharacter::OnFire()
{
    if (bIsRunning == false)
    {
        auto& TimerManager = GetWorld()->GetTimerManager();

        // Start the firing timer and use the remaining time of the previous timer.
        float RemainingTime = FMath::Max(TimerManager.GetTimerRemaining(FireTimer), 0.0f);
        TimerManager.SetTimer(FireTimer, this, &ACaptureTheFlagCharacter::Server_Fire, 1.0f / FireRate, true, RemainingTime);
    }
}

// Stops weapon firing
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

// Called when clearing weapons fire timer
void ACaptureTheFlagCharacter::ClearFireTimer()
{
    // Clear the timer after a delay set in ReleaseTrigger() function.
    GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

// Handles moving forward/backward
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

// Handles stafing movement, left and right
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

// Called via input to turn at a given rate
void ACaptureTheFlagCharacter::TurnAtRate(float Rate)
{
    // Calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

// Called via input to turn look up / down at a given rate
void ACaptureTheFlagCharacter::LookUpAtRate(float Rate)
{
    // Calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// Handles Crouching
void ACaptureTheFlagCharacter::Crouching()
{
    bIsCrouching = true;
    AnimationInstance->bIsCrouched = bIsCrouching;
}

// Stops Crouching
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

// Handles Aiming 
void ACaptureTheFlagCharacter::Aim()
{
    bIsAiming = true;
    AnimationInstance->bIsAiming = bIsAiming;
    OverShoulderCamera->SetFieldOfView(60);
}

// Stops Aiming
void ACaptureTheFlagCharacter::StopAim()
{
    bIsAiming = false;
    AnimationInstance->bIsAiming = bIsAiming;
    OverShoulderCamera->SetFieldOfView(90);
}

// Handles Running
void ACaptureTheFlagCharacter::Run()
{
    bIsRunning = true;
    bIsCrouching = false;
    AnimationInstance->bIsRunning = bIsRunning;
    AnimationInstance->bIsCrouched = bIsCrouching;
    GetCharacterMovement()->MaxWalkSpeed = 750.0f;
}

// Stops Running
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

class ACaptureTheFlagGameState* ACaptureTheFlagCharacter::GetGameState()
{
    return Cast<ACaptureTheFlagGameState>(GetWorld()->GetGameState());
}

ACaptureTheFlagPlayerState* ACaptureTheFlagCharacter::GetCharacterPlayerState()
{
    return Cast<class ACaptureTheFlagPlayerState>(GetPlayerState());
}

// Called when setting carried flag
void ACaptureTheFlagCharacter::SetCarriedFlag(AActor* flag)
{
    Flag = Cast<AFlag>(flag);
}

void ACaptureTheFlagCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACaptureTheFlagCharacter, FireRate);
    DOREPLIFETIME(ACaptureTheFlagCharacter, bIsAiming);
    DOREPLIFETIME(ACaptureTheFlagCharacter, bIsRunning);
    DOREPLIFETIME(ACaptureTheFlagCharacter, bIsCrouching);
    DOREPLIFETIME(ACaptureTheFlagCharacter, AimPitch);
    DOREPLIFETIME(ACaptureTheFlagCharacter, AimYaw);

    DOREPLIFETIME(ACaptureTheFlagCharacter, PlayerName);
    DOREPLIFETIME(ACaptureTheFlagCharacter, PlayerTeam);
    DOREPLIFETIME(ACaptureTheFlagCharacter, NetIndex);

    DOREPLIFETIME(ACaptureTheFlagCharacter, TeamOneCount);
    DOREPLIFETIME(ACaptureTheFlagCharacter, TeamTwoCount);
    DOREPLIFETIME(ACaptureTheFlagCharacter, TeamOneScore);
    DOREPLIFETIME(ACaptureTheFlagCharacter, TeamTwoScore);
    DOREPLIFETIME(ACaptureTheFlagCharacter, PostBeginPlayDelay);

    DOREPLIFETIME(ACaptureTheFlagCharacter, DefaultTPMaterials);
    DOREPLIFETIME(ACaptureTheFlagCharacter, Flag);
}