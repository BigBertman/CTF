// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagGameMode.h"
#include "CaptureTheFlagHUD.h"
#include "CaptureTheFlagCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACaptureTheFlagGameMode::ACaptureTheFlagGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ACaptureTheFlagHUD::StaticClass();
}
