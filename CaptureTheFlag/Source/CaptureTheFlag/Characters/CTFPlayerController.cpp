// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFPlayerController.h"
#include "CaptureTheFlagCharacter.h"
#include "../UI/PlayerWidget.h"

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

void ACTFPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, "AMyPlayerController::Possess: - " + aPawn->GetName());
}

void ACTFPlayerController::OnUnPossess()
{
    Super::OnUnPossess();

    APawn* posessedPawn = Cast<APawn>(GetOwner());
    if (posessedPawn)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, "AMyPlayerController::UnPossess: - " + GetOwner()->GetName());
    }
}

void ACTFPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void ACTFPlayerController::AcknowledgePossession(APawn* PossessedPawn)
{
    Super::AcknowledgePossession(PossessedPawn);

    Character = Cast<ACaptureTheFlagCharacter>(PossessedPawn);

    // Set up gameplay key bindings
    check(Character != nullptr && "Possesed a pawn that isn't of a desired character type!");

    if (Character != nullptr)
    {
        // Bind jump events
        InputComponent->BindAction("Jump", IE_Pressed, Character, &ACharacter::Jump);
        InputComponent->BindAction("Jump", IE_Released, Character, &ACharacter::StopJumping);

        // Bind movement events
        InputComponent->BindAxis("MoveForward", Character, &ACaptureTheFlagCharacter::MoveForward);
        InputComponent->BindAxis("MoveRight", Character, &ACaptureTheFlagCharacter::MoveRight);

        // We have 2 versions of the rotation bindings to handle different kinds of devices differently
        // "turn" handles devices that provide an absolute delta, such as a mouse.
        // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
        InputComponent->BindAxis("Turn", Character, &APawn::AddControllerYawInput);
        InputComponent->BindAxis("TurnRate", Character, &ACaptureTheFlagCharacter::TurnAtRate);
        InputComponent->BindAxis("LookUp", Character, &APawn::AddControllerPitchInput);
        InputComponent->BindAxis("LookUpRate", Character, &ACaptureTheFlagCharacter::LookUpAtRate);
    }
}

void ACTFPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GameInfoWidget != nullptr)
    {
        GameInfoWidget->UpdateHealthBar();
    }
}