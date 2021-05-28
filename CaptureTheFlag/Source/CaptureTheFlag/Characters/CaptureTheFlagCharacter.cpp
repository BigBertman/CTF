// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagCharacter.h"
#include "../CaptureTheFlag.h"
#include "../CaptureTheFlagProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "CharacterBaseAnimation.h"
#include <GameFramework/PlayerController.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ACaptureTheFlagCharacter

ACaptureTheFlagCharacter::ACaptureTheFlagCharacter() :
    SkeletalMesh(nullptr),
    AnimationInstance(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // GetCharacterMovement()->MaxWalkSpeed = 280.0f;

    GetMesh()->SetOwnerNoSee(true);
    GetMesh()->SetIsReplicated(true);

    // set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // Create a CameraComponent	
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
    Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

    // Create a gun mesh component
    FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
    FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
    FP_Gun->bCastDynamicShadow = false;
    FP_Gun->CastShadow = false;
    // FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
    FP_Gun->SetupAttachment(RootComponent);

    FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    FP_MuzzleLocation->SetupAttachment(FP_Gun);
    FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

    // Default offset from the character location for projectiles to spawn
    GunOffset = FVector(100.0f, 0.0f, 10.0f);


    // Create a gun mesh component
    TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
    //TP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
    TP_Gun->SetOwnerNoSee(true);
    TP_Gun->bCastDynamicShadow = false;
    TP_Gun->CastShadow = false;
    TP_Gun->SetIsReplicated(true);
    TP_Gun->SetupAttachment(RootComponent);

    // Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
    // are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

    // Create VR Controllers.
    R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
    R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
    R_MotionController->SetupAttachment(RootComponent);
    L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
    L_MotionController->SetupAttachment(RootComponent);

    // Create a gun and attach it to the right-hand VR controller.
    // Create a gun mesh component
    VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
    VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
    VR_Gun->bCastDynamicShadow = false;
    VR_Gun->CastShadow = false;
    VR_Gun->SetupAttachment(R_MotionController);
    VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    VR_Gun->SetIsReplicated(true);

    VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
    VR_MuzzleLocation->SetupAttachment(VR_Gun);
    VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
    VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.    

    // Uncomment the following line to turn motion controllers on by default:
//bUsingMotionControllers = true;

    FireRate = 10.0f;

    bReplicates = true;
    SetReplicates(true);
    SetReplicateMovement(true);
    GetCharacterMovement()->SetIsReplicated(true);
    GetCharacterMovement()->SetNetAddressable();
}

void ACaptureTheFlagCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

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
    FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
    TP_Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));

    // Show or hide the two versions of the gun based on whether or not we're using motion controllers.
    if (bUsingMotionControllers)
    {
        VR_Gun->SetHiddenInGame(false, true);
        Mesh1P->SetHiddenInGame(true, true);
    }
    else
    {
        VR_Gun->SetHiddenInGame(true, true);
        Mesh1P->SetHiddenInGame(false, true);
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

}

void ACaptureTheFlagCharacter::NMC_PlayWeaponFiringAnimation_Implementation()
{
    AnimationInstance->Montage_Play(FireAnimation);
    AnimationInstance->Montage_Play(FireAnimationTP);

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
            if (bUsingMotionControllers)
            {
                const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
                const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
                World->SpawnActor<ACaptureTheFlagProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
            }
            else
            {
                const FRotator SpawnRotation = GetControlRotation();
                // MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
                const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

                //Set Spawn Collision Handling Override
                FActorSpawnParameters ActorSpawnParams;
                ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

                // spawn the projectile at the muzzle
                World->SpawnActor<ACaptureTheFlagProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

               // FVector MuzzleLocation = TP_Gun->GetSocketLocation("Muzzle") + SpawnRotation.RotateVector(GunOffset);

                //World->SpawnActor<ACaptureTheFlagProjectile>(ProjectileClass, MuzzleLocation, SpawnRotation, ActorSpawnParams);


            }
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

void ACaptureTheFlagCharacter::OnResetVR()
{
    UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACaptureTheFlagCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    if (TouchItem.bIsPressed == true)
    {
        return;
    }
    if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
    {
        Server_Fire();
    }
    TouchItem.bIsPressed = true;
    TouchItem.FingerIndex = FingerIndex;
    TouchItem.Location = Location;
    TouchItem.bMoved = false;
}

void ACaptureTheFlagCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    if (TouchItem.bIsPressed == false)
    {
        return;
    }
    TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ACaptureTheFlagCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

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

bool ACaptureTheFlagCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
    if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
    {
        PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ACaptureTheFlagCharacter::BeginTouch);
        PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ACaptureTheFlagCharacter::EndTouch);

        //Commenting this out to be more consistent with FPS BP template.
        //PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ACaptureTheFlagCharacter::TouchUpdate);
        return true;
    }

    return false;
}

void ACaptureTheFlagCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACaptureTheFlagCharacter, FireRate);
}