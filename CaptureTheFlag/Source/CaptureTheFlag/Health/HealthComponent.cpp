// Copyright Bryson Bertuzzi, Inc. All Rights Reserved.

#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    bWantsInitializeComponent = true;

    // Set default values.
    MaximumHealth = 100.0f;
    CurrentHealth = -1.0f;
}

// Called when initializing the component.
void UHealthComponent::InitializeComponent()
{
    Super::InitializeComponent();

    SetIsReplicated(true);

    // Sanitize default values.
    MaximumHealth = FMath::Max(1.0f, MaximumHealth);
    CurrentHealth = FMath::Min(CurrentHealth, MaximumHealth);

    // Set current health to maximum health.
    if (CurrentHealth < 0.0f)
    {
        CurrentHealth = MaximumHealth;
    }

    GetOwner()->SetCanBeDamaged(true);
    // Subscribe to the delegate in the attached actor.
    GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeAnyDamage);
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    // Broadcast actor's death event if initial health is zero.
    if (CurrentHealth == 0.0f)
    {
        OnDeath.Broadcast(nullptr);
    }
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Called when the actor is damaged in any way
void UHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    // Check if alrady dead.
    if (IsDead())
    {
        return;
    }

    CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaximumHealth);

    // Handle current health value reaching zero.
    if (CurrentHealth <= 0.0f)
    {
        // Broadcast actor's death event.
        OnDeath.Broadcast(DamageCauser);
    }
}

// Called when checking if player is dead
bool UHealthComponent::IsDead() const
{
    return CurrentHealth <= 0.0f;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UHealthComponent, CurrentHealth);
}