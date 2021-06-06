// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include "../Health/HealthComponent.h"

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
}

// Update HUD for Health Bar
void UPlayerWidget::UpdateHealthBar()
{
    UProgressBar* HealthBar = Cast<UProgressBar>(GetWidgetFromName("HealthBar"));

    ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    float MaximumHealth = Character->GetHealth()->MaximumHealth;
    float CurrentHealth = Character->GetHealth()->CurrentHealth;
    if (Character != nullptr)
    {
        if (HealthBar != nullptr)
        {
            HealthBar->SetPercent(CurrentHealth / MaximumHealth);
        }
    }
}