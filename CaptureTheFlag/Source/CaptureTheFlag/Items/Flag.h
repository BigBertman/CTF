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

    /** Primitive Component */
    class UPrimitiveComponent* Primitive;

    /** Mesh Component */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class  UStaticMeshComponent* MeshComponent;

    /** Shape Component */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class  UShapeComponent* CollisionComponent;

    /** Team colour in numbers */
    UPROPERTY(EditAnywhere, Replicated)
        int TeamFlag;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Callled after all components have been initialized
    virtual void PostInitializeComponents() override;

public:
    UFUNCTION(NetMulticast, Reliable)
        virtual void NMC_Disable();

    UFUNCTION(NetMulticast, Reliable)
        virtual void NMC_Enable();

    /**
    * Called when enabling or disabling custom depth
    * @param selected - enables or disables custom depth of object
    */
    void ShowSelect(bool selected);

    /**
    * Called when settting the flag states
    * @param NewState - New flag state to change current state
    * @param Pawn - Player
    */
    void SetState(ECaptureFlagState NewState, AActor* Pawn);

    /**
    * Called when something enters the sphere component
    * @param OverlappedActor - Current Actor being overlapped
    * @param OtherActor - Actor overlapping
    */
    UFUNCTION()
        void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
private:

    /** Captrue Flag State */
    UPROPERTY(Replicated)
        ECaptureFlagState State;
};
