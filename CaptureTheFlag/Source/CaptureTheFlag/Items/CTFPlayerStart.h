// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CTFPlayerStart.generated.h"

/**
 *
 */
UCLASS()
class CAPTURETHEFLAG_API ACTFPlayerStart : public APlayerStart
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Defaults")
        int bTeamBlue;

};
