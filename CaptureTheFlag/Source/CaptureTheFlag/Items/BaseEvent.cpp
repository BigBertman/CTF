// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#include "BaseEvent.h"
#include "Components/BoxComponent.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include "../GameMode/CaptureTheFlagGameMode.h"
#include "Flag.h"

// Sets default values
ABaseEvent::ABaseEvent()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Collision Component
    {
        CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
        RootComponent = CollisionComponent;
    }

    SetReplicates(true);
}

// Called when the game starts or when spawned
void ABaseEvent::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        OnActorBeginOverlap.AddDynamic(this, &ABaseEvent::OnOverlapBegin);
    }
}

// Called every frame
void ABaseEvent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called when something enters the box component
void ABaseEvent::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    if (OtherActor != nullptr)
    {
        ACaptureTheFlagCharacter* player = Cast<ACaptureTheFlagCharacter>(OtherActor);

        if (player != nullptr && player->Flag != nullptr)
        {
            UWorld* World = GetWorld();

            if (World != nullptr)
            {
                // Get GameMode from World
                ACaptureTheFlagGameMode* GM = World->GetAuthGameMode<ACaptureTheFlagGameMode>();

                if (GM != nullptr)
                {
                    // Add Score to Players Team
                    GM->AddScore(player->PlayerTeam);

                    // Set Flag state to InBase
                    player->Flag->SetState(ECaptureFlagState::InBase, this);

                    player->Flag = nullptr;
                }
            }
        }
    }
}