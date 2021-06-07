// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBaseAnimation.h"
#include "Net/UnrealNetwork.h"

UCharacterBaseAnimation::UCharacterBaseAnimation() :
    bIsMoving(false),
    MovementSpeed(0.0f),
    StrafingRotation(0.0f),
    OwningPawn(nullptr),
    bIsAiming(false),
    bIsRunning(false),
    bIsCrouched(false)
{
}

void UCharacterBaseAnimation::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwningPawn = TryGetPawnOwner();
}

void UCharacterBaseAnimation::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
}

void UCharacterBaseAnimation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UCharacterBaseAnimation, bIsMoving);
    DOREPLIFETIME(UCharacterBaseAnimation, MovementSpeed);
    DOREPLIFETIME(UCharacterBaseAnimation, StrafingRotation);
    DOREPLIFETIME(UCharacterBaseAnimation, bIsAiming);
    DOREPLIFETIME(UCharacterBaseAnimation, bIsRunning);
    DOREPLIFETIME(UCharacterBaseAnimation, bIsCrouched);
}