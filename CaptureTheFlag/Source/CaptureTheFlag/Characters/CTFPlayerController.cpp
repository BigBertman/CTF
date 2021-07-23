// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#include "CTFPlayerController.h"
#include "CaptureTheFlagCharacter.h"
#include "../UI/PlayerWidget.h"
#include "../GameMode/CaptureTheFlagGameMode.h"

// Called when the owning Actor begins play
void ACTFPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (GameInfoBP != nullptr && IsLocalController())
    {
        GameInfoWidget = CreateWidget<UPlayerWidget>(this, GameInfoBP);
        GameInfoWidget->AddToViewport();
        GameInfoWidget->UpdateHealthBar();
    }
}

// Called when pawn is controlled
void ACTFPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, "ACTFPlayerController::Possess: - " + aPawn->GetName());
}

// Called when pawn is not controlled
void ACTFPlayerController::OnUnPossess()
{
    Super::OnUnPossess();

    APawn* posessedPawn = Cast<APawn>(GetOwner());
    if (posessedPawn)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, "ACTFPlayerController::UnPossess: - " + GetOwner()->GetName());
    }
}

// Called when setting up custom input bindings.
void ACTFPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

// Called when pawn is being controlled
void ACTFPlayerController::AcknowledgePossession(APawn* PossessedPawn)
{
    Super::AcknowledgePossession(PossessedPawn);

    Character = Cast<ACaptureTheFlagCharacter>(PossessedPawn);

    // Set up gameplay key bindings
    check(Character != nullptr && "Possesed a pawn that isn't of a desired character type!");

    if (Character != nullptr)
    {
        // Bind jump events
        InputComponent->BindAction("Jump", IE_Pressed, Character, &ACaptureTheFlagCharacter::Jump);
        InputComponent->BindAction("Jump", IE_Released, Character, &ACaptureTheFlagCharacter::StopJumping);

        // Bind movement events
        InputComponent->BindAxis("MoveForward", Character, &ACaptureTheFlagCharacter::Server_MoveForward);
        InputComponent->BindAxis("MoveRight", Character, &ACaptureTheFlagCharacter::Server_MoveRight);

        // We have 2 versions of the rotation bindings to handle different kinds of devices differently
        // "turn" handles devices that provide an absolute delta, such as a mouse.
        // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
        InputComponent->BindAxis("Turn", Character, &APawn::AddControllerYawInput);
        InputComponent->BindAxis("TurnRate", Character, &ACaptureTheFlagCharacter::TurnAtRate);
        InputComponent->BindAxis("LookUp", Character, &APawn::AddControllerPitchInput);
        InputComponent->BindAxis("LookUpRate", Character, &ACaptureTheFlagCharacter::LookUpAtRate);

        // Bind fire event
        InputComponent->BindAction("Fire", IE_Pressed, Character, &ACaptureTheFlagCharacter::OnFire);
        InputComponent->BindAction("Fire", IE_Released, Character, &ACaptureTheFlagCharacter::StopFire);

        // Bind crouch event
        InputComponent->BindAction("Crouch", IE_Pressed, Character, &ACaptureTheFlagCharacter::Server_Crouch);
        InputComponent->BindAction("Crouch", IE_Released, Character, &ACaptureTheFlagCharacter::Server_StopCrouch);

        // Bind aim event
        InputComponent->BindAction("Aim", IE_Pressed, Character, &ACaptureTheFlagCharacter::Aim);
        InputComponent->BindAction("Aim", IE_Released, Character, &ACaptureTheFlagCharacter::StopAim);

        // Bind Sprint event
        InputComponent->BindAction("Sprint", IE_Pressed, Character, &ACaptureTheFlagCharacter::Server_Run);
        InputComponent->BindAction("Sprint", IE_Released, Character, &ACaptureTheFlagCharacter::Server_StopRun);
    }
}

// Called every frame
void ACTFPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GameInfoWidget != nullptr)
    {
        GameInfoWidget->UpdateHealthBar();
    }
}