// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/Blaster_HUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterType/CombatState.h"
#include "CombatComponent.generated.h"

#define TRACE_LEN 80000.f


class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	friend class ABlasterCharacter;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	void EquippedWeapon(AWeapon* WeaponToEquipped);

	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishedReload();
	
	void FireButtonPressed(bool bPressed);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);


protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
		void ServerReload();
	
	void HandleReload();

	UFUNCTION()
		void OnRep_EquippedWeapon();


	void Fire();
	
	UFUNCTION(Server,Reliable)
		void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
		void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshair(FHitResult& TraceHitResult);

	void SetHudCrossHair(float DeltaTime);

	int32 AmountToReload();

private:	

	UPROPERTY()
		class ABlasterCharacter*			CombatCharacter;
	UPROPERTY()
		class ABlaster_PlayerController*	PlayerController;
	UPROPERTY()
		class ABlaster_HUD*					PlayerHUD;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
		class AWeapon* _EquippedWeapon;
	
	UPROPERTY(Replicated)
		bool bAiming;
	
	UPROPERTY(EditAnywhere)
		float BaseWalkSpeed=600.f;
	
	UPROPERTY(EditAnywhere)
		float AimWalkSpeed=350.f;

	bool bFireButtonPressed;
	
	

	bool bCanFire = true;


	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	float DefaultFOV;
	float CurrentFOV;
	
	UPROPERTY(EditAnywhere,Category=Combat)
		float ZoomFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = Combat)
		float ZoomInterpSpeed = 20.f;

	
	
	void InterpFOV(float Deltatime);

	FTimerHandle FireTimer;

	void StartFireTimer();
	void FinishFireTimer();

	bool CanFire();

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
		int32 CarriedAmmo;
	
	UFUNCTION()
		void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32>CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
		int32 StartingARAmmo=0;
	
	UPROPERTY(EditAnywhere)
		int32 StartingRocketAmmo=0;

	UPROPERTY(EditAnywhere)
		int32 StartingPistolAmmo=0;

	void InitializeCarriedAmmo();


	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
		ECombatState CombatState= ECombatState::ECS_Unoccupied;
	
	UFUNCTION()
		void OnRep_CombatState();

	void UpdateAmmoValue();
};
