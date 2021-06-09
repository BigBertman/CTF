// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagGameMode.h"
#include "../CaptureTheFlagHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "GameFramework/PlayerStart.h"

ACaptureTheFlagGameMode::ACaptureTheFlagGameMode()
{
    // set default pawn class to our Blueprinted character
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

    // use our custom HUD class
    HUDClass = ACaptureTheFlagHUD::StaticClass();
}

void ACaptureTheFlagGameMode::RespawnPlayer(APlayerController* NewPlayer)
{
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
    APawn* pawn = SpawnDefaultPawnFor(NewPlayer, PlayerStarts[0]);
    if (pawn != nullptr)
    {
        if (Cast<ACaptureTheFlagCharacter>(pawn))
        {
            NewPlayer->SetPawn(pawn);
            RestartPlayer(NewPlayer);
        }
    }
}
