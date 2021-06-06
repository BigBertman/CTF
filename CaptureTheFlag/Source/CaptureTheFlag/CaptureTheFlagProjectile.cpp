// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/CaptureTheFlagCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"

ACaptureTheFlagProjectile::ACaptureTheFlagProjectile()
{
    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;

    // Die after 3 seconds by default
    //InitialLifeSpan = 3.0f;

    // Default projectile parameters.
    Damage = 10.0f;

    SetReplicates(true);
    SetReplicateMovement(true);
}

void ACaptureTheFlagProjectile::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    AActor::OnActorHit.AddDynamic(this, &ACaptureTheFlagProjectile::OnActorHit);
}

void ACaptureTheFlagProjectile::OnActorHit(AActor* Self, AActor* Other, FVector NormalImpulse, const FHitResult& Hit)
{
    // Cause damage to the hit actor.
    if (Other != nullptr)
    {
        //  Other->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

        ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(Other);

        if (Character != nullptr)
        {
            Character->ApplyDamage(this->GetInstigator());
        }
    }

    Destroy();
}
