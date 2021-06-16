// Fill out your copyright notice in the Description page of Project Settings.

#include "Flag.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include "Net/UnrealNetwork.h"
#include "FlagBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AFlag::AFlag()
    : State(ECaptureFlagState::Loose)
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SetRootComponent(CollisionComponent);

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SetReplicates(true);
    SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AFlag::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        OnActorBeginOverlap.AddDynamic(this, &AFlag::OnOverlapBegin);
        SetState(ECaptureFlagState::InBase, nullptr);
    }
}

// Called every frame
void AFlag::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFlag::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Get the root primitive component.
    Primitive = Cast<UPrimitiveComponent>(GetRootComponent());
}

void AFlag::NMC_Enable_Implementation()
{
    if (Primitive != nullptr)
    {
        SetActorHiddenInGame(false);
        Primitive->SetActive(true);
    }
}

void AFlag::NMC_Disable_Implementation()
{
    if (Primitive != nullptr)
    {
        SetActorHiddenInGame(true);
        Primitive->SetActive(false);
    }
}

void AFlag::ShowSelect(bool selected)
{
    if (Primitive != nullptr)
    {
        Primitive->SetRenderCustomDepth(selected);
    }
}

void AFlag::SetState(ECaptureFlagState NewState, AActor* Pawn)
{
    switch (State)
    {
    case ECaptureFlagState::Carried:
    {
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CollisionComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        CollisionComponent->SetRelativeLocation(Pawn->GetActorLocation());
        break;
    }

    }

    switch (NewState)
    {
    case ECaptureFlagState::Carried:
    {
        ACaptureTheFlagCharacter* Player = Cast<ACaptureTheFlagCharacter>(Pawn);
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CollisionComponent->AttachToComponent(Player->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName(TEXT("Flag")));
        Player->SetCarriedFlag(this);
        break;
    }

    case ECaptureFlagState::InBase:
    {
        UWorld* World = GetWorld();

        if (World != nullptr)
        {
            TArray<AActor*> FlagBaseArray;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlagBase::StaticClass(), FlagBaseArray);

            for (AActor* FlagBase : FlagBaseArray)
            {
                AFlagBase* FB = Cast<AFlagBase>(FlagBase);

                if (FB != nullptr)
                {
                    if (TeamFlag == 0)
                    {
                        if (FB->ActorHasTag("RedBase"))
                        {
                            FRotator SpawnRotation = FB->GetActorRotation();
                            FVector SpawnLocation = FB->GetActorLocation();
                            SpawnLocation.Z += 130.0f;

                            SetActorLocation(SpawnLocation);
                            SetActorRotation(SpawnRotation);
                        }
                    }

                    if (TeamFlag == 1)
                    {
                        if (FB->ActorHasTag("BlueBase"))
                        {
                            FRotator SpawnRotation = FB->GetActorRotation();
                            FVector SpawnLocation = FB->GetActorLocation();
                            SpawnLocation.Z += 130.0f;

                            SetActorLocation(SpawnLocation);
                            SetActorRotation(SpawnRotation);
                        }
                    }
                }
            }
        }

        NewState = ECaptureFlagState::Loose;
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        break;
    }

    case ECaptureFlagState::Loose:
    {
        break;
    }

    }

    State = NewState;
}

void AFlag::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    if (Cast<ACaptureTheFlagCharacter>(OtherActor) != nullptr)
    {
        if (State == ECaptureFlagState::Loose)
        {
            ACaptureTheFlagCharacter* Player = Cast<ACaptureTheFlagCharacter>(OtherActor);
            if (Player != nullptr)
            {
                if (Player->PlayerTeam != TeamFlag)
                {
                    SetState(ECaptureFlagState::Carried, OtherActor);
                }
            }
        }
    }
}

void AFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFlag, State);
    DOREPLIFETIME(AFlag, TeamFlag);
}