// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CaptureTheFlagCharacter.generated.h"

class UInputComponent;

UCLASS(config = Game)
class ACaptureTheFlagCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    /** Pawn mesh: 1st person view (arms; seen only by self) */
    //UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    //    class USkeletalMeshComponent* Mesh1P;

    /** Gun mesh: 3st person view (seen only by self) */
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
        class USkeletalMeshComponent* TP_Gun;

    /** Location on gun mesh where projectiles should spawn. */
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
        class USceneComponent* FP_MuzzleLocation;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* OverShoulderCamera;

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class USpringArmComponent* OverShoulderCameraBoom;

    void ApplyDamage(AActor* DamageCauser);

public:
    ACaptureTheFlagCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void PostInitializeComponents() override;

    UFUNCTION(NetMulticast, Unreliable)
        void NMC_PlayWeaponFiringAnimation();

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_Fire();

    UFUNCTION()
        virtual void OnDeath(AActor* KilledBy);
    UFUNCTION(NetMulticast, Reliable)
        void NMC_OnClientDeath();

public:
    /** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        float BaseTurnRate;

    /** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        float BaseLookUpRate;

    /** Gun muzzle's offset from the characters location */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        FVector GunOffset;

    /** Projectile class to spawn */
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class ACaptureTheFlagProjectile> ProjectileClass;

    /** Sound to play each time we fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        class USoundBase* FireSound;

    /** AnimMontage to play each time we fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        class UAnimMontage* FireAnimation;

    UPROPERTY(Replicated, EditDefaultsOnly)
        float FireRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class UHealthComponent* Health;

public:
    /** Fires a projectile. */
    void OnFire();

    void StopFire();

    /** Handles moving forward/backward */
    void MoveForward(float Val);

    /** Handles stafing movement, left and right */
    void MoveRight(float Val);

    /**
     * Called via input to turn at a given rate.
     * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void TurnAtRate(float Rate);

    /**
     * Called via input to turn look up/down at a given rate.
     * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void LookUpAtRate(float Rate);

    //class UHealthComponent* GetHealth();

    void Crouch();
    void StopCrouch();

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_Crouch();

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_StopCrouch();

    void Aim();

    void StopAim();

    void Run();
    void StopRun();

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_Run();

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_StopRun();

    UPROPERTY(Replicated)
        bool bIsAiming = false;

    UPROPERTY(Replicated)
        bool bIsRunning = false;

    UPROPERTY(Replicated)
        bool bIsCrouching = false;


protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    // End of APawn interface

public:
    /** Returns Mesh1P subobject **/
    //FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetOverShoulderCameraBoom() const { return OverShoulderCameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetOverShoulderCamera() const { return OverShoulderCamera; }
    /** Returns HealthComponent subobject **/
    FORCEINLINE class UHealthComponent* GetHealth() { return Health; }

private:
    void ClearFireTimer();

    FTimerHandle FireTimer;

    class UCharacterBaseAnimation* AnimationInstance;
    class USkeletalMeshComponent* SkeletalMesh;


};

