// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterBaseAnimation.generated.h"

/**
 *
 */
UCLASS()
class CAPTURETHEFLAG_API UCharacterBaseAnimation : public UAnimInstance
{
    GENERATED_BODY()

public:
    /** Is Player Moving */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        bool bIsMoving;

    /** Movement Speed */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        float MovementSpeed;

    /** Strafing Rotation */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        float StrafingRotation;

    /** Is Player Aiming */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        bool bIsAiming;

    /** Is Player Running */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        bool bIsRunning;

    /** Is Player Crouching */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        bool bIsCrouched;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        float AimPitch;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        float AimYaw;

protected:
    UCharacterBaseAnimation();

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
    /** Owning Pawn */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        APawn* OwningPawn;
};