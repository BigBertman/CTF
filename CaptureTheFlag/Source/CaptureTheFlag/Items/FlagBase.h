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

    /** Mesh Component */
    UPROPERTY(EditAnywhere)
        class UStaticMeshComponent* MeshComponent;

    /** Red Flag Template */
    UPROPERTY(EditAnywhere, Replicated)
        TSubclassOf<class AFlag> RedFlag;

    /** Blue Flag Template */
    UPROPERTY(EditAnywhere, Replicated)
        TSubclassOf<class AFlag> BlueFlag;

    /** Team base in numbers */
    UPROPERTY(EditAnywhere, Replicated)
        int TeamBase;

    /**
    * Called when reseting flag
    * @param flag - flag being called
    */
    void ResetFlag(class AFlag* flag);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

};
