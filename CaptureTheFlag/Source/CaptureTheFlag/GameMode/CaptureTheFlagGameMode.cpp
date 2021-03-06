// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#include "CaptureTheFlagGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../CaptureTheFlagHUD.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include "../GameState/CaptureTheFlagGameState.h"
#include "../PlayerState/CaptureTheFlagPlayerState.h"
#include "../Items/CTFPlayerStart.h"
#include "EngineUtils.h"


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
    ChoosePlayerStart(NewPlayer);
}

// Called when respawning player
void ACaptureTheFlagGameMode::RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex)
{
    APawn* pawn = SpawnDefaultPawnFor(NewPlayer, ChoosePlayerStart(NewPlayer));

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
AActor* ACaptureTheFlagGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    if (Player != nullptr)
    {
        ACaptureTheFlagPlayerState* PS = Cast<ACaptureTheFlagPlayerState>(Player->PlayerState);

        if (PS != nullptr)
        {
            TArray<ACTFPlayerStart*> Starts;
            for (TActorIterator<ACTFPlayerStart> it(GetWorld()); it; ++it)
            {
                if (it->bTeamBlue == PS->PlayerTeam)
                {
                    Starts.Add(*it);
                }
            }
            return Starts[Starts.Num() - 1];
        }
    }
    return NULL;
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