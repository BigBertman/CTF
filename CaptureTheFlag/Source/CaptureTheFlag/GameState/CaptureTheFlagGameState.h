// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CaptureTheFlagGameState.generated.h"



USTRUCT()
struct FCaptureTeam
{
    GENERATED_BODY()

        //TArray<class ACaptureTheFlagCharacter*> mPlayers;
};

/**
 *
 */
UCLASS()
class CAPTURETHEFLAG_API ACaptureTheFlagGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    //ACaptureTheFlagGameState(const FObjectInitializer& ObjectInitializer);

    //void AddToTeam(class ACaptureTheFlagCharacter* Character);
    //void RemoveFromTeam(class ACaptureTheFlagCharacter* Character);

    //int32 GetNextTeamIndex() const;

    //UPROPERTY(Transient, Replicated)
    //    TArray<FCaptureTeam> mTeams;

    //UPROPERTY(Replicated)
    //    int BlueScore;

    //UPROPERTY(Replicated)
    //    int RedScore;

};
