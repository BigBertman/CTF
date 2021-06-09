// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlagGameState.h"
#include "../GameMode/CaptureTheFlagGameMode.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include "Net/UnrealNetwork.h"

//ACaptureTheFlagGameState::ACaptureTheFlagGameState(const FObjectInitializer& ObjectInitializer)
//    : Super(ObjectInitializer)
//{
//    for (int32 i = 0; i < (int32)ECaptureTeam::MAX; ++i)
//    {
//        mTeams.AddDefaulted();
//    }
//
//    RedScore = 0;
//    BlueScore = 0;
//}
//
//void ACaptureTheFlagGameState::AddToTeam(ACaptureTheFlagCharacter* Character)
//{
//    if (Character != nullptr)
//    {
//        int32 TeamToAddIndex = GetNextTeamIndex();
//        mTeams[TeamToAddIndex].mPlayers.Add(Character);
//        Character->SetTeam((ECaptureTeam)TeamToAddIndex);
//    }
//}
//
//void ACaptureTheFlagGameState::RemoveFromTeam(ACaptureTheFlagCharacter* Character)
//{
//	if (Character == nullptr)
//	{
//		return;
//	}
//
//	ECaptureTeam AssignedTeam = Character->GetTeam();
//
//	if (AssignedTeam == ECaptureTeam::Invalid)
//	{
//		return;
//	}
//
//	const int32 AssignedTeamIndex = (int32)AssignedTeam;
//	mTeams[AssignedTeamIndex].mPlayers.Remove(Character);
//}
//
//int32 ACaptureTheFlagGameState::GetNextTeamIndex() const
//{
//	int32 NumPlayersSmallest = mTeams[0].mPlayers.Num();
//	int32 NumPlayersSmallestIndex = 0;
//
//	for (int32 i = 1; i < (int32)ECaptureTeam::MAX; ++i)
//	{
//		const int32 NumPlayers = mTeams[i].mPlayers.Num();
//
//		//IF NumPlayers LESS THAN NumPlayersSmallest
//		if (NumPlayers < NumPlayersSmallest)
//		{
//			NumPlayersSmallest = NumPlayers;
//			NumPlayersSmallestIndex = i;
//		}
//	}
//	return NumPlayersSmallestIndex;
//}
//
//void ACaptureTheFlagGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//    DOREPLIFETIME(ACaptureTheFlagGameState, RedScore);
//    DOREPLIFETIME(ACaptureTheFlagGameState, BlueScore);
//    DOREPLIFETIME(ACaptureTheFlagGameState, mTeams);
//}