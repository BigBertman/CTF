// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CTFPlayerController.generated.h"

/**
 *
 */
UCLASS()
class CAPTURETHEFLAG_API ACTFPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void SetupInputComponent() override;

    virtual void AcknowledgePossession(APawn* PossessedPawn) override;

    virtual void OnPossess(APawn* aPawn) override;

    virtual void OnUnPossess() override;

    virtual void BeginPlay() override;

protected:
    class ACaptureTheFlagCharacter* Character;
};