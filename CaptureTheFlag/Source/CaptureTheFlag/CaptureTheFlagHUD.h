// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CaptureTheFlagHUD.generated.h"

UCLASS()
class ACaptureTheFlagHUD : public AHUD
{
    GENERATED_BODY()

public:
    ACaptureTheFlagHUD();

    /** Primary draw call for the HUD */
    virtual void DrawHUD() override;

private:
    /** Crosshair asset pointer */
    class UTexture2D* CrosshairTex;

};

