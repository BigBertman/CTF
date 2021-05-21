// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagGameMode.h"
#include "../CaptureTheFlagHUD.h"
#include "UObject/ConstructorHelpers.h"

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
