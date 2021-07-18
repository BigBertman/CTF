// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#include "CharacterBaseAnimation.h"
#include "Net/UnrealNetwork.h"

UCharacterBaseAnimation::UCharacterBaseAnimation() :
    bIsMoving(false),
    MovementSpeed(0.0f),
    StrafingRotation(0.0f),
    OwningPawn(nullptr),
    bIsAiming(false),
    bIsRunning(false),
    bIsCrouched(false),
    AimPitch(0.0f),
    AimYaw(0.0f)
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
    DOREPLIFETIME(UCharacterBaseAnimation, AimPitch);
    DOREPLIFETIME(UCharacterBaseAnimation, AimYaw);
}