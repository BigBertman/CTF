// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlagBase.generated.h"

UCLASS()
class CAPTURETHEFLAG_API AFlagBase : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AFlagBase();

    UPROPERTY(EditAnywhere)
        class UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditAnywhere, Replicated)
        TSubclassOf<class AFlag> RedFlag;

    UPROPERTY(EditAnywhere, Replicated)
        TSubclassOf<class AFlag> BlueFlag;

    UPROPERTY(EditAnywhere, Replicated)
        int TeamBase;

    void SpawnFlag(int team);

    void ResetFlag(class AFlag* flag);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

};
