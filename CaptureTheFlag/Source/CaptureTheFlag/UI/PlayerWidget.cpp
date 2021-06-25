// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#include "PlayerWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include "../Health/HealthComponent.h"
#include "../GameState/CaptureTheFlagGameState.h"

// Called when initializing widget
bool UPlayerWidget::Initialize()
{
    bool bResult = Super::Initialize();

    if (!bResult)
    {
        return false;
    }

    // Health Bar
    {
        UProgressBar* HealthBar = Cast<UProgressBar>(GetWidgetFromName("HealthBar"));

        if (HealthBar != nullptr)
        {
            HealthBar->SetPercent(0.8f);
        }
    }

    return true;
}

// Called every frame
void UPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateHealthBar();
    UpdateTeamScore();
}

// Update HUD for Health Bar
void UPlayerWidget::UpdateHealthBar()
{
    UProgressBar* HealthBar = Cast<UProgressBar>(GetWidgetFromName("HealthBar"));

    ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Get player's maximum health
    float MaximumHealth = Character->GetHealth()->MaximumHealth;
    // Get player's current health
    float CurrentHealth = Character->GetHealth()->CurrentHealth;

    if (Character != nullptr)
    {
        if (HealthBar != nullptr)
        {
            // Set HealthBar Percent based on CurrentHealth / MaximumHealth
            HealthBar->SetPercent(CurrentHealth / MaximumHealth);
        }
    }
}

// Update HUD for Team Score
void UPlayerWidget::UpdateTeamScore()
{
    UWorld* World = GetWorld();

    if (World != nullptr)
    {
        ACaptureTheFlagGameState* GS = World->GetGameState<ACaptureTheFlagGameState>();

        if (GS != nullptr)
        {
            // Red Team Score
            {
                FString RedValue = FString::Printf(TEXT("Red Score: %d"), GS->TeamOneScore);

                UTextBlock* RedWidget = Cast<UTextBlock>(GetWidgetFromName("Red"));

                if (RedWidget != nullptr)
                {
                    RedWidget->SetText(FText::FromString(RedValue));
                }
            }

            // Blue Team Score
            {
                FString BlueValue = FString::Printf(TEXT("Blue Score: %d"), GS->TeamTwoScore);

                UTextBlock* BlueWidget = Cast<UTextBlock>(GetWidgetFromName("Blue"));

                if (BlueWidget != nullptr)
                {
                    BlueWidget->SetText(FText::FromString(BlueValue));
                }
            }
        }
    }
}