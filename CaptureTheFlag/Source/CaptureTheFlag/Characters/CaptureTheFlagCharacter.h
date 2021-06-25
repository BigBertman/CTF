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
    // Sets default values for this character's properties
    ACaptureTheFlagCharacter();

    /** Gun mesh: 3st person view */
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

    /**
    * Called when applying damage to player
    * @param DamageCauser - Actor causing damage
    */
    void ApplyDamage(AActor* DamageCauser);

protected:
    // Called when the owning Actor begins play
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Callled after all components have been initialized
    virtual void PostInitializeComponents() override;

    // Called when playing weapon animation
    UFUNCTION(NetMulticast, Unreliable)
        void NMC_PlayWeaponFiringAnimation();

    // Called when firing weapon
    UFUNCTION(Server, Reliable, WithValidation)
        void Server_Fire();

    /**
    * Called when player dies
    * @param KilledBy - Actor responsable for death
    */
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

    /** Weapon Fire Rate */
    UPROPERTY(Replicated, EditDefaultsOnly)
        float FireRate;

    /** Health Component */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class UHealthComponent* Health;

    /** Team Flag */
    UPROPERTY(Replicated)
        class AFlag* Flag;

public:
    // Called when setting carried flag
    void SetCarriedFlag(AActor* flag);

    // Fires a projectile
    void OnFire();

    // Stops weapon firing
    void StopFire();

    // Handles moving forward/backward
    void MoveForward(float Val);

    UFUNCTION(NetMulticast, Reliable)
        void NMC_MoveForward(float Val);

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_MoveForward(float Val);

    // Handles stafing movement, left and right
    void MoveRight(float Val);

    UFUNCTION(NetMulticast, Reliable)
        void NMC_MoveRight(float Val);

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_MoveRight(float Val);

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

    // Handles Crouching
    void Crouching();

    // Stops Crouching
    void StopCrouch();

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_Crouch();
    UFUNCTION(Server, Reliable, WithValidation)
        void Server_StopCrouch();

    UFUNCTION(NetMulticast, Reliable)
        void NMC_Crouch();
    UFUNCTION(NetMulticast, Reliable)
        void NMC_StopCrouch();

    // Handles Aiming 
    void Aim();

    // Stops Aiming
    void StopAim();

    // Handles Running
    void Run();

    // Stops Running
    void StopRun();

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_Run();
    UFUNCTION(Server, Reliable, WithValidation)
        void Server_StopRun();

    UFUNCTION(NetMulticast, Reliable)
        void NMC_Run();
    UFUNCTION(NetMulticast, Reliable)
        void NMC_StopRun();

    /** Is Player Aiming */
    UPROPERTY(Replicated)
        bool bIsAiming = false;

    /** Is Player Running */
    UPROPERTY(Replicated)
        bool bIsRunning = false;

    /** Is Player Crouching */
    UPROPERTY(Replicated)
        bool bIsCrouching = false;

    // Called when respawning player
    void Respawn();

    /** The player's name (usually loaded from a save file or database) */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Name", Replicated)
        FString PlayerName;

    /** The player's current team */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Gameplay", Replicated)
        int PlayerTeam;

    /** Size of team one(grabbed from game state) */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Debug", Replicated)
        int TeamOneCount;

    /** Size of team two(grabbed from game state) */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Debug", Replicated)
        int TeamTwoCount;

    /** Team one's score (grabbed from game state) */
    UPROPERTY(BlueprintReadWrite, Category = "Character|Debug", Replicated)
        int TeamOneScore;

    /** Team two's score (grabbed from game state) */
    UPROPERTY(BlueprintReadWrite, Category = "Character|Debug", Replicated)
        int TeamTwoScore;

    /** Used to reference the player's current netID */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Debug", Replicated)
        int NetIndex;

    /** Players Material Color Template */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Mesh", Replicated)
        TArray<class UMaterialInterface*> DefaultTPMaterials;

    // Getters for PlayerState and GameState
    class ACaptureTheFlagPlayerState* GetCharacterPlayerState();
    class ACaptureTheFlagGameState* GetGameState();

    // Assigns a team to the player that logs in
    void AssignTeams();

    // Assigns a Network Index to the player that logs in
    void AssignNetIndex();

    // Multicasts to all clients to assign the team color when player logs in
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_AssignTeamsColor(int team, int index);

    // Sets the material to the mesh
    void SetMaterialToMesh(USkeletalMeshComponent* InMeshComp, const TArray<UMaterialInterface*>& InMaterials);

    UFUNCTION(Server, Reliable, WithValidation)
        void Server_PostBeginPlay();

    UFUNCTION(BlueprintCallable, Category = Game)
        virtual void UpdateAndCheckPlayer();

protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    // End of APawn interface

    // Getter for SkeletalMesh
    class USkeletalMeshComponent* GetSkeletalMesh() { return SkeletalMesh; }

public:
    /** Returns Mesh1P subobject */
    //FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

    /** Returns CameraBoom subobject */
    FORCEINLINE class USpringArmComponent* GetOverShoulderCameraBoom() const { return OverShoulderCameraBoom; }

    /** Returns FollowCamera subobject */
    FORCEINLINE class UCameraComponent* GetOverShoulderCamera() const { return OverShoulderCamera; }

    /** Returns HealthComponent subobject */
    FORCEINLINE class UHealthComponent* GetHealth() { return Health; }

private:
    // Called when clearing weapons fire timer
    void ClearFireTimer();

    /** Weapon Fire Timer */
    FTimerHandle FireTimer;

    /** Character Animation */
    class UCharacterBaseAnimation* AnimationInstance;

    /** Skeletal Mesh Component */
    class USkeletalMeshComponent* SkeletalMesh;

    /** Timer Handle */
    FTimerHandle UpdateHandle;

    /** Post Begin Delay Timer */
    UPROPERTY(Replicated)
        FTimerHandle PostBeginPlayDelay;
};

