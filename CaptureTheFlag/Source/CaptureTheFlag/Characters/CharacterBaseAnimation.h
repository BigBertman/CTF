// Fill out your copyright notice in the Description page of Project Settings.

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        bool bIsMoving;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float MovementSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float StrafingRotation;

protected:
    UCharacterBaseAnimation();

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
    APawn* OwningPawn;
};
