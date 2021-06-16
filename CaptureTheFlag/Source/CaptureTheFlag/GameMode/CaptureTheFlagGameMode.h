// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CaptureTheFlagGameMode.generated.h"

UCLASS(minimalapi)
class ACaptureTheFlagGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACaptureTheFlagGameMode();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
        int TeamOneScore = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
        int TeamTwoScore = 0;

    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    void RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex);

    UFUNCTION(Reliable, NetMulticast)
        void AddScore(int team);

private:
    void HandleNewPlayer(APlayerController* NewPlayer);

};



