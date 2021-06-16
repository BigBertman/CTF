// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flag.generated.h"

UENUM()
enum class ECaptureFlagState : uint8
{
    Carried = 0,
    InBase,
    Loose
};

UCLASS()
class CAPTURETHEFLAG_API AFlag : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AFlag();

    class UPrimitiveComponent* Primitive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class  UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class  UShapeComponent* CollisionComponent;

    UPROPERTY(EditAnywhere, Replicated)
        int TeamFlag;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void PostInitializeComponents() override;

public:
    UFUNCTION(NetMulticast, Reliable)
        virtual void NMC_Disable();

    UFUNCTION(NetMulticast, Reliable)
        virtual void NMC_Enable();

    void ShowSelect(bool selected);

    void SetState(ECaptureFlagState NewState, AActor* Pawn);

    UFUNCTION()
        void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
private:
    UPROPERTY(Replicated)
        ECaptureFlagState State;
};
