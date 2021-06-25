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

    /** Maximum Health */
    UPROPERTY(EditAnywhere)
        float MaximumHealth;

    /** Current Health */
    UPROPERTY(EditAnywhere, Replicated)
        float CurrentHealth;

    /** Death Signature */
    UPROPERTY(BlueprintAssignable)
        FOnDeathSignature OnDeath;

    // Called when checking if player is dead
    UFUNCTION()
        bool IsDead() const;

protected:
    // Called when initializing the component.
    virtual void InitializeComponent() override;

    // Called when the game starts
    virtual void BeginPlay() override;

    /**
    * Called when the actor is damaged in any way.
    * @param DamagedActor - Current Actor being affected
    * @param Damage - How much damage to apply
    * @param DamageType - Type of Damage
    * @param InstigatedBy - Controller responsible for damage
    * @param DamageCauser - The Actor that directly caused the damage
    */
    UFUNCTION()
        void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
