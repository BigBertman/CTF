// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CaptureTheFlagGameState.generated.h"

/**
 *
 */
UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials, Replicated)
        TArray<class UMaterialInterface*> TeamOnePMaterials;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials, Replicated)
        TArray<class UMaterialInterface*> TeamTwoPMaterials;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
        bool TeamsEnabled;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int TeamOneScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int TeamTwoScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int TeamOneMatchScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int TeamTwoMatchScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int TeamOneSize = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int TeamTwoSize = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        float GameTime;
};