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
    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
    //    bool bIsAiming;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        bool bIsMoving;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        float MovementSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        float StrafingRotation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        bool bIsAiming;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        bool bIsRunning;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        bool bIsCrouched;

protected:
    UCharacterBaseAnimation();

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
    APawn* OwningPawn;
};
