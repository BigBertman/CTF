// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#include "FlagBase.h"
#include "Components/StaticMeshComponent.h"
#include "Flag.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFlagBase::AFlagBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Flag Base Mesh
    {
        MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    }

    SetReplicates(true);
}

// Called when the game starts or when spawned
void AFlagBase::BeginPlay()
{
    Super::BeginPlay();
}

// Called when reseting flag
void AFlagBase::ResetFlag(AFlag* flag)
{
    FRotator SpawnRotation = GetActorRotation();
    FVector SpawnLocation = GetActorLocation();
    SpawnLocation.Z += 10.0f;

    flag->SetActorLocation(SpawnLocation);
    flag->SetActorRotation(SpawnRotation);
}

// Called every frame
void AFlagBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFlagBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFlagBase, TeamBase);
}