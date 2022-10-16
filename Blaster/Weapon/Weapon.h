// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	EWS_Initial UMETA(DisplayName="Initial State"),
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_Dropped UMETA(DisplayName="Dropped"),
	
	
	EWS_MAX UMETA(DisplayName="DefaultMAX"),

};


UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	void ShowPickupWidget(bool bShowWidget);
	void SetHUDAmmo();
	virtual void Fire(const FVector& HitTarget);

	void Dropped();
	bool EmptyAmmo() { return Ammo<=0; }
	void AddAmmo(int32 AmmoToAdd);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnSphereOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherCom,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherCom,
		int32 OtherBodyIndex

	);

	virtual void OnRep_Owner()override;
private:

	UPROPERTY(VisibleAnywhere,Category="Weapon", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponMesh;
	
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AreaSphere;


	UPROPERTY(ReplicatedUsing=OnRep_WeaponState, VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		EWeaponState WeaponState;


	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickUpWidget;
	
	
	UFUNCTION()
		void OnRep_WeaponState();

	UFUNCTION()
		void OnRep_Ammo();

	void SpendRound();

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		class UAnimationAsset* FireAnimation;


	UPROPERTY(EditAnywhere)
		TSubclassOf<class AAmmoShell> ShellClass;

	UPROPERTY(EditAnywhere)
		float ZoomFOV = 30.f;


	UPROPERTY(EditAnywhere)
		float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere,ReplicatedUsing=OnRep_Ammo,Category=Ammo)
		int32 Ammo;
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo, Category = Ammo)
		int32 MaxAmmo;


	UPROPERTY()
		class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
		class ABlaster_PlayerController* BlasterOwnerController;


	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;


public:
	 void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere()const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh()const { return WeaponMesh; }
	FORCEINLINE float GetZoomFOV()const { return ZoomFOV; }
	FORCEINLINE float GetZoomInterpSpeed()const { return ZoomInterpSpeed; }
	FORCEINLINE EWeaponType GetWeaponType()const { return WeaponType; }
	FORCEINLINE int32 GetAmmo()const { return Ammo; }
	FORCEINLINE int32 GetMaxAmmo()const { return MaxAmmo; }


	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairCenter;
	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairLeft;
	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairRight;
	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairTop;
	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairBottom;
	UPROPERTY(EditAnywhere)
		class USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, Category = Combat)
		bool bAutomatic = true;

	UPROPERTY(EditAnywhere, Category = Combat)
		float FireDelay = .15f;



};
