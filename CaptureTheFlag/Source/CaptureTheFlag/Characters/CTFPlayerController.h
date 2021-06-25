// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

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

public:
    /** Player Widget */
    UPROPERTY()
        class UPlayerWidget* GameInfoWidget;

    /** Player Widget Template */
    UPROPERTY(EditDefaultsOnly)
        TSubclassOf<UPlayerWidget> GameInfoBP;

protected:
    // Called when setting up custom input bindings.
    virtual void SetupInputComponent() override;

    // Called when pawn is being controlled
    virtual void AcknowledgePossession(APawn* PossessedPawn) override;

    // Called when pawn is controlled
    virtual void OnPossess(APawn* aPawn) override;

    // Called when pawn is not controlled
    virtual void OnUnPossess() override;

    // Called when the owning Actor begins play
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:

    /** Character */
    class ACaptureTheFlagCharacter* Character;
};