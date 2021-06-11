// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CaptureTheFlagPlayerState.generated.h"

/**
 *
 */
UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagPlayerState : public APlayerState
{
    GENERATED_BODY()

public:

    UFUNCTION()
        virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere)
        float GameTime;

    UPROPERTY()
        float RespawnTimer;

    UPROPERTY(VisibleAnywhere)
        int PlayerHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        int Index;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        int playerScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        int MatchScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
        int PlayerTeam;
};