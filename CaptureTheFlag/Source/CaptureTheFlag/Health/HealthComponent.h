// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, KilledBy);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CAPTURETHEFLAG_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UHealthComponent();

    UPROPERTY(EditAnywhere)
        float MaximumHealth;

    UPROPERTY(EditAnywhere, Replicated)
        float CurrentHealth;

    UPROPERTY(BlueprintAssignable)
        FOnDeathSignature OnDeath;

    UFUNCTION()
        bool IsDead() const;

protected:

    virtual void InitializeComponent() override;

    // Called when the game starts
    virtual void BeginPlay() override;

    UFUNCTION()
        void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
