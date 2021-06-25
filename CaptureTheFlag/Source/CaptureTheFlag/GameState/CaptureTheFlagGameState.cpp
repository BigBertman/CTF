// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureTheFlagGameState.h"
#include "Net/UnrealNetwork.h"

void ACaptureTheFlagGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACaptureTheFlagGameState, TeamsEnabled);
    DOREPLIFETIME(ACaptureTheFlagGameState, TeamOneScore);
    DOREPLIFETIME(ACaptureTheFlagGameState, TeamTwoScore);

    DOREPLIFETIME(ACaptureTheFlagGameState, TeamOneMatchScore);
    DOREPLIFETIME(ACaptureTheFlagGameState, TeamTwoMatchScore);

    DOREPLIFETIME(ACaptureTheFlagGameState, TeamOneSize);
    DOREPLIFETIME(ACaptureTheFlagGameState, TeamTwoSize);
    DOREPLIFETIME(ACaptureTheFlagGameState, GameTime);

    DOREPLIFETIME(ACaptureTheFlagGameState, TeamOnePMaterials);
    DOREPLIFETIME(ACaptureTheFlagGameState, TeamTwoPMaterials);
}