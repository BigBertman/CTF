// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagGameMode.h"
#include "../CaptureTheFlagHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "GameFramework/PlayerStart.h"
#include <CaptureTheFlag/GameState/CaptureTheFlagGameState.h>
#include "../PlayerState/CaptureTheFlagPlayerState.h"

ACaptureTheFlagGameMode::ACaptureTheFlagGameMode()
{
    // Set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
    if (PlayerPawnClassFinder.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnClassFinder.Class;
    }

    static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/BP_PlayerController"));
    if (PlayerControllerClassFinder.Class != NULL)
    {
        PlayerControllerClass = PlayerControllerClassFinder.Class;
    }

    static ConstructorHelpers::FClassFinder<AGameStateBase> GameStateBPClass(TEXT("/Game/FirstPersonCPP/Blueprints/BP_CaptureTheFlagGameState"));
    if (GameStateBPClass.Class != NULL)
    {
        GameStateClass = GameStateBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<APlayerState> PlayerStateBPClass(TEXT("/Game/FirstPersonCPP/Blueprints/BP_CaptureTheFlagPlayerState"));
    if (PlayerStateBPClass.Class != NULL)
    {
        PlayerStateClass = PlayerStateBPClass.Class;
    }

    // use our custom HUD class
    HUDClass = ACaptureTheFlagHUD::StaticClass();
}

void ACaptureTheFlagGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
    HandleNewPlayer(NewPlayer);
}

void ACaptureTheFlagGameMode::RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex)
{
    TArray<AActor*> PlayerStarts;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    APawn* pawn = SpawnDefaultPawnFor(NewPlayer, PlayerStarts[0]);

    if (pawn != nullptr)
    {
        if (Cast<ACaptureTheFlagCharacter>(pawn))
        {
            Cast<ACaptureTheFlagCharacter>(pawn)->PlayerTeam = playerTeam;

            Cast<ACaptureTheFlagCharacter>(pawn)->NetIndex = NetIndex;

            NewPlayer->SetPawn(pawn);

            RestartPlayer(NewPlayer);
        }
    }
}


void ACaptureTheFlagGameMode::HandleNewPlayer(APlayerController* NewPlayer)
{
    ACaptureTheFlagCharacter* character = Cast<ACaptureTheFlagCharacter>(NewPlayer->GetPawn());

    if (character != nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "CHaracter Logged In");
        character->AssignTeams();
        character->AssignNetIndex();
    }
}