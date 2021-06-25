// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

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

    /** Team One Score */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
        int TeamOneScore = 0;

    /** Team Two Score */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
        int TeamTwoScore = 0;

    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    /**
    * Called when respawning player.
    * @param NewPlayer - Player being added to the game
    * @param playerTeam - Which team is the player
    * @param NetIndex - Network index
    */
    void RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex);

    // Called when adding score to teams
    UFUNCTION(Reliable, NetMulticast)
        void AddScore(int team);

private:
    // Called when dealing with new players
    void HandleNewPlayer(APlayerController* NewPlayer);
};