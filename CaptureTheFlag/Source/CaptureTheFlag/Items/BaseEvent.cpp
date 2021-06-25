// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEvent.h"
#include "Components/BoxComponent.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include "../GameMode/CaptureTheFlagGameMode.h"
#include "Flag.h"

// Sets default values
ABaseEvent::ABaseEvent()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;

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
                ACaptureTheFlagGameMode* GS = World->GetAuthGameMode<ACaptureTheFlagGameMode>();

                if (GS != nullptr)
                {
                    GS->AddScore(player->PlayerTeam);

                    player->Flag->SetState(ECaptureFlagState::InBase, this);

                    player->Flag = nullptr;
                }
            }
        }
    }
}