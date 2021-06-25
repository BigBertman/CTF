// Fill out your copyright notice in the Description page of Project Settings.

#include "Flag.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "FlagBase.h"
#include "../Characters/CaptureTheFlagCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFlag::AFlag()
    : State(ECaptureFlagState::Loose)
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Collision Component
    {
        CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        SetRootComponent(CollisionComponent);
    }

    // Flag Mesh
    {
        MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
        MeshComponent->SetupAttachment(RootComponent);
        MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

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

// Callled after all components have been initialized
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

// Called when enabling or disabling custom depth
void AFlag::ShowSelect(bool selected)
{
    if (Primitive != nullptr)
    {
        Primitive->SetRenderCustomDepth(selected);
    }
}

// Called when settting the flag states
void AFlag::SetState(ECaptureFlagState NewState, AActor* Pawn)
{
    switch (State)
    {
        // If Flag is being pickup.
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
        // If Flag is being pickup by player.
    case ECaptureFlagState::Carried:
    {
        ACaptureTheFlagCharacter* Player = Cast<ACaptureTheFlagCharacter>(Pawn);
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CollisionComponent->AttachToComponent(Player->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName(TEXT("Flag")));
        Player->SetCarriedFlag(this);
        break;
    }
    // If Flag is returning to base.
    case ECaptureFlagState::InBase:
    {
        UWorld* World = GetWorld();

        if (World != nullptr)
        {
            // Find all Flag Bases in map.
            TArray<AActor*> FlagBaseArray;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlagBase::StaticClass(), FlagBaseArray);

            for (AActor* FlagBase : FlagBaseArray)
            {
                AFlagBase* FB = Cast<AFlagBase>(FlagBase);

                if (FB != nullptr)
                {
                    // If Flag is Red Team...
                    if (TeamFlag == 0)
                    {
                        if (FB->ActorHasTag("RedBase"))
                        {
                            // Return to Flag Base...
                            FRotator SpawnRotation = FB->GetActorRotation(); // Spawn Rotation of Flag.
                            FVector SpawnLocation = FB->GetActorLocation(); // Spawn Location of Flag.
                            SpawnLocation.Z += 130.0f;

                            // Set Flag Location and Rotation.
                            SetActorLocation(SpawnLocation);
                            SetActorRotation(SpawnRotation);
                        }
                    }
                    // If Flag is Blue Team...
                    if (TeamFlag == 1)
                    {
                        if (FB->ActorHasTag("BlueBase"))
                        {
                            // Return to Flag Base...
                            FRotator SpawnRotation = FB->GetActorRotation(); // Spawn Rotation of Flag.
                            FVector SpawnLocation = FB->GetActorLocation(); // Spawn Location of Flag.
                            SpawnLocation.Z += 130.0f;

                            // Set Flag Location and Rotation.
                            SetActorLocation(SpawnLocation);
                            SetActorRotation(SpawnRotation);
                        }
                    }
                }
            }
        }
        // Set Flag State to Loose
        NewState = ECaptureFlagState::Loose;
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        break;
    }
    // If Flag is returning to base.
    case ECaptureFlagState::Loose:
    {
        break;
    }
    }

    State = NewState;
}

// Called when something enters the sphere component
void AFlag::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    if (Cast<ACaptureTheFlagCharacter>(OtherActor) != nullptr)
    {
        // If Flag State is Loose...
        if (State == ECaptureFlagState::Loose)
        {
            ACaptureTheFlagCharacter* Player = Cast<ACaptureTheFlagCharacter>(OtherActor);
            if (Player != nullptr)
            {
                if (Player->PlayerTeam != TeamFlag)
                {
                    // Set Flag State to Carried.
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