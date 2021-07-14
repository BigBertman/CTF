// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#include "CaptureTheFlagGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../CaptureTheFlagHUD.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include "../GameState/CaptureTheFlagGameState.h"
#include "../PlayerState/CaptureTheFlagPlayerState.h"

ACaptureTheFlagGameMode::ACaptureTheFlagGameMode()
{
    // Set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
    if (PlayerPawnClassFinder.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnClassFinder.Class;
    }

    // Set default player controller class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/Gamemode/BP_PlayerController"));
    if (PlayerControllerClassFinder.Class != NULL)
    {
        PlayerControllerClass = PlayerControllerClassFinder.Class;
    }

    // Set default game state class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<AGameStateBase> GameStateBPClass(TEXT("/Game/FirstPersonCPP/Blueprints/Gamemode/BP_CaptureTheFlagGameState"));
    if (GameStateBPClass.Class != NULL)
    {
        GameStateClass = GameStateBPClass.Class;
    }

    // Set default player state class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APlayerState> PlayerStateBPClass(TEXT("/Game/FirstPersonCPP/Blueprints/Gamemode/BP_CaptureTheFlagPlayerState"));
    if (PlayerStateBPClass.Class != NULL)
    {
        PlayerStateClass = PlayerStateBPClass.Class;
    }

    // Use our custom HUD class
    HUDClass = ACaptureTheFlagHUD::StaticClass();
}

void ACaptureTheFlagGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
    HandleNewPlayer(NewPlayer);
}

// Called when respawning player
void ACaptureTheFlagGameMode::RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex)
{
    // Get All Player Starts in World
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    APawn* pawn = SpawnDefaultPawnFor(NewPlayer, PlayerStarts[0]);

    for (AActor* it : PlayerStarts)
    {
        if (playerTeam == 0 && it->ActorHasTag("Red Base"))
        {
            pawn = SpawnDefaultPawnFor(NewPlayer, it);
        }

        if (playerTeam == 1 && it->ActorHasTag("Blue Base"))
        {
            pawn = SpawnDefaultPawnFor(NewPlayer, it);
        }
    }

    if (pawn != nullptr)
    {
        if (Cast<ACaptureTheFlagCharacter>(pawn))
        {
            Cast<ACaptureTheFlagCharacter>(pawn)->PlayerTeam = playerTeam;

            Cast<ACaptureTheFlagCharacter>(pawn)->NetIndex = NetIndex;

            // Set New Player Pawn to pawn
            NewPlayer->SetPawn(pawn);

            // Restart Player
            RestartPlayer(NewPlayer);
        }
    }
}

// Called when dealing with new players
void ACaptureTheFlagGameMode::HandleNewPlayer(APlayerController* NewPlayer)
{
    ACaptureTheFlagCharacter* character = Cast<ACaptureTheFlagCharacter>(NewPlayer->GetPawn());

    if (character != nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Character Logged In");
        character->AssignTeams();
        character->AssignNetIndex();
    }
}

// Called when adding score to teams
void ACaptureTheFlagGameMode::AddScore_Implementation(int team)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        UWorld* World = GetWorld();

        if (World != nullptr)
        {
            ACaptureTheFlagGameState* GS = World->GetGameState<ACaptureTheFlagGameState>();

            switch (team)
            {
            case 0:
                GS->TeamOneScore++;
                break;
            case 1:
                GS->TeamTwoScore++;
                break;
            default:
                break;
            }
        }
    }
}