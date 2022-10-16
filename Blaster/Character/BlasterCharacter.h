// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterType/TurningPlace.h"
#include "Blaster/Interfaces/InteractCrosshairInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterType/CombatState.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractCrosshairInterface
{
	GENERATED_BODY()

public:
	
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
	virtual void PostInitializeComponents()override;
	virtual void Jump()override;
	virtual void OnRep_ReplicatedMovement()override;
	virtual void Destroyed()override;
	
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	void PlayReloadMontage();

	void Elim();


	
	UFUNCTION(NetMulticast, Reliable)
		void MultiCastElim();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Equipped();
	void CrouchButtonPress();
	void CrouchButtonRealse();
	void AimButtonPress();
	void AimButtonRealse();
	void ReloadButtonPress();
	void AimOffSet(float DeltaTime);
	void CaculateAOPitch();
	void SimProxiesTurn();
	void FireButtonPress();
	void FireButtonRealesed();
	void PlayHitReactMontage();


	UFUNCTION()
		void ReceiveDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,class AController* InstigatorController,AActor* DamageCauser);
	
	void UpdateHUDHp();
	
	void PollInit();
	void RotateInPlace(float DeltaTime);


private:
	UPROPERTY(VisibleAnywhere,Category=Camera)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* PlayerCamera;


	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
		class UWidgetComponent* OverHeadWidget;


	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;

	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* Combat;


	UFUNCTION(Server,Reliable)
		void ServerEquipButtonPressed();
	
	UPROPERTY(EditAnywhere,Category="Player Stats")
		float Max_Hp = 100.f;
	UPROPERTY(ReplicatedUsing=OnRep_Hp,VisibleAnywhere, Category = "Player Stats")
		float Current_Hp=100.f;

	UFUNCTION()
		void OnRep_Hp();



	float AO_Yaw;
	float AO_Pitch;
	float InterpAO_Yaw;

	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere,Category=Combat)
		class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere)
		class UAnimMontage* HitReactMontage;
	
	UPROPERTY(EditAnywhere)
		class UAnimMontage* ElimMontage;


	UPROPERTY(EditAnywhere)
		class UAnimMontage* ReloadMontage;

	void HideCameraIfCharacterClose();
	
	UPROPERTY(EditAnywhere)
		float CameraThreshold = 200.f;

	bool bRotateRootBone;
	bool bElimmed=false;
	float TurnThreadhold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotationCurrentFrame;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CaculateSpeed();

	UPROPERTY(EditDefaultsOnly)
		float ElimDelay = 3.f;



	UPROPERTY()
		class ABlaster_PlayerController* BlasterPlayerController;
	
	
	FTimerHandle ElimTimer;

	void ElimTimerFinished();
	
	
	UPROPERTY(EditAnywhere)
		UCurveFloat* DissolveCurve;
	
	
	UPROPERTY(VisibleAnywhere)
		UTimelineComponent* DissolveTimeline;
	
	FOnTimelineFloat DissolveTrack;

	UFUNCTION()
		void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	UPROPERTY(VisibleAnywhere,Category=Elim)
		UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* DissolveMaterialInstance;

	UPROPERTY(EditAnywhere)
		UParticleSystem* ElimBotEffect;
	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
		class USoundCue* ElimBotSound;

	UPROPERTY()
		class ABlasterPlayerState* BlasterPlayerState;


public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	FORCEINLINE float GetAOYaw()const { return AO_Yaw; }
	FORCEINLINE float GetAOPitch()const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetPlayerCamera() const { return PlayerCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHp() const { return Current_Hp; }
	FORCEINLINE float GetMaxHp() const { return Max_Hp; }
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	ECombatState GetCombatState()const;
	AWeapon* GetEquippedWeapon();

	FVector GetHitTarget()const;

	bool IsWeaponEquipped();
	bool IsAiming();
	
	UPROPERTY(Replicated)
		bool bDisableGameplay=false;

};
