// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureTheFlagProjectile.generated.h"

UCLASS(config = Game)
class ACaptureTheFlagProjectile : public AActor
{
    GENERATED_BODY()

public:
    /** Sphere collision component */
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
        class USphereComponent* CollisionComp;

    /** Projectile movement component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        class UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditDefaultsOnly)
        float Damage;

public:
    ACaptureTheFlagProjectile();

    /** called when projectile hits something */
    UFUNCTION()
        void OnActorHit(AActor* Self, AActor* Other, FVector NormalImpulse, const FHitResult& Hit);

    /** Returns CollisionComp subobject **/
    FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
    /** Returns ProjectileMovement subobject **/
    FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
    virtual void PostInitializeComponents() override;

};

