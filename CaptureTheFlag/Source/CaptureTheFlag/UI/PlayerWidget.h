// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 *
 */
UCLASS()
class CAPTURETHEFLAG_API UPlayerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    // Called every frame
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // Update HUD for Health Bar
    virtual void UpdateHealthBar();
};
