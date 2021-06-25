// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEvent.generated.h"

UCLASS()
class CAPTURETHEFLAG_API ABaseEvent : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABaseEvent();

    /** Box Component */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class UBoxComponent* CollisionComponent;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    /**
    * Called when something enters the box component
    * @param OverlappedActor - Current Actor being overlapped
    * @param OtherActor - Actor overlapping
    */
    UFUNCTION()
        void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};
