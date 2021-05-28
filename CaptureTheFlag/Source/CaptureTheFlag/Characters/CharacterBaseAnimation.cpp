// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBaseAnimation.h"
#include "Net/UnrealNetwork.h"

UCharacterBaseAnimation::UCharacterBaseAnimation() :
    //bIsAiming(false),
    bIsMoving(false),
    MovementSpeed(0.0f),
    StrafingRotation(0.0f),
    OwningPawn(nullptr)
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
