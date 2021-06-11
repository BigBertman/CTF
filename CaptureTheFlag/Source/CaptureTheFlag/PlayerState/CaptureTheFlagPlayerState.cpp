// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlagPlayerState.h"
#include "Net/UnrealNetwork.h"

void ACaptureTheFlagPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACaptureTheFlagPlayerState, playerScore);
    DOREPLIFETIME(ACaptureTheFlagPlayerState, PlayerTeam);
    DOREPLIFETIME(ACaptureTheFlagPlayerState, MatchScore);
}

void ACaptureTheFlagPlayerState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
