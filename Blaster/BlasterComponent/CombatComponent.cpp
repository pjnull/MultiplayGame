
#include "CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Blaster/PlayerController/Blaster_PlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	//PrimaryComponentTick.bStartWithTickEnabled = true;
	//PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	//RegisterComponent();
	//RegisterAllComponentTickFunctions(true);
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;

	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, _EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (CombatCharacter)
	{
		CombatCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		
		if (CombatCharacter->GetPlayerCamera())
		{
			DefaultFOV = CombatCharacter->GetPlayerCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;

		}

		if (CombatCharacter->HasAuthority())
		{
			InitializeCarriedAmmo();

		}
	}

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CombatCharacter && CombatCharacter->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;
		SetHudCrossHair(DeltaTime);
		InterpFOV(DeltaTime);
	}
}



void UCombatComponent::FireButtonPressed(bool bPressed)
{
	

	bFireButtonPressed = bPressed;
	
	if (bFireButtonPressed)
	{
		bCanFire = true;
		Fire();
	}
}

void UCombatComponent::Fire()
{
	
	if (CanFire())
	{
		
		bCanFire = false;

		ServerFire(HitTarget);

		if (_EquippedWeapon)
		{
			CrosshairShootingFactor += 0.75f;

		}
		StartFireTimer();


	}
}

bool UCombatComponent::CanFire()
{
	if (_EquippedWeapon == nullptr)return false;
	return !_EquippedWeapon->EmptyAmmo() && bCanFire&& CombatState==ECombatState::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	PlayerController = PlayerController == nullptr ? Cast<ABlaster_PlayerController>(CombatCharacter->Controller) : PlayerController;

	if (PlayerController)
	{
		PlayerController->Set_HUD_CarriedAmmo(CarriedAmmo);
	}

}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle,StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher,StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol,StartingPistolAmmo);
}


void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MultiCastFire(TraceHitTarget);
}

void UCombatComponent::MultiCastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (_EquippedWeapon == nullptr)return;
	if (CombatCharacter&&CombatState==ECombatState::ECS_Unoccupied)
	{
		CombatCharacter->PlayFireMontage(bAiming);
		_EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::StartFireTimer()
{
	if (_EquippedWeapon == nullptr||CombatCharacter==nullptr)return;
	CombatCharacter->GetWorldTimerManager().SetTimer
											(
												FireTimer,this, 
												&UCombatComponent::FinishFireTimer, _EquippedWeapon->FireDelay
											);

}

void UCombatComponent::FinishFireTimer()
{
	if (_EquippedWeapon == nullptr)return;
	bCanFire = true;
	if (bFireButtonPressed&& _EquippedWeapon->bAutomatic)Fire();

	if (_EquippedWeapon->EmptyAmmo())
	{
		Reload();
	}

}



void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (CombatCharacter)
		{
			float DistanceToCharacter = (CombatCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter+100.f);
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LEN;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (TraceHitResult.GetActor()&& TraceHitResult.GetActor()->Implements<UInteractCrosshairInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
	}
}


void UCombatComponent::EquippedWeapon(AWeapon* WeaponToEquipped)
{
	if (CombatCharacter == nullptr || WeaponToEquipped == nullptr)return;
	
	if (_EquippedWeapon)
	{
		_EquippedWeapon->Dropped();
	}
	_EquippedWeapon = WeaponToEquipped;
	_EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket=CombatCharacter->GetMesh()->GetSocketByName(FName("RightHand_Socket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(_EquippedWeapon,CombatCharacter->GetMesh());
	}
	
	_EquippedWeapon->SetOwner(CombatCharacter);
	_EquippedWeapon->SetHUDAmmo();
	
	if (CarriedAmmoMap.Contains(_EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[_EquippedWeapon->GetWeaponType()];
	}
	PlayerController = PlayerController == nullptr ? Cast<ABlaster_PlayerController>(CombatCharacter->Controller) : PlayerController;
	
	if (PlayerController)
	{
		PlayerController->Set_HUD_CarriedAmmo(CarriedAmmo);
	}

	if (_EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation
		(
			this,
			_EquippedWeapon->EquipSound,
			CombatCharacter->GetActorLocation()
		);
	}

	if (_EquippedWeapon->EmptyAmmo())
	{
		Reload();
	}


	CombatCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	CombatCharacter->bUseControllerRotationYaw = true;
}


void UCombatComponent::OnRep_EquippedWeapon()
{
	if (_EquippedWeapon&& CombatCharacter)
	{
		_EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		const USkeletalMeshSocket* HandSocket = CombatCharacter->GetMesh()->GetSocketByName(FName("RightHand_Socket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(_EquippedWeapon, CombatCharacter->GetMesh());
		}

		CombatCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		CombatCharacter->bUseControllerRotationYaw = true;

		if (_EquippedWeapon->EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation
			(
				this,
				_EquippedWeapon->EquipSound,
				CombatCharacter->GetActorLocation()
			);
		}


	}
}

void UCombatComponent::Reload()
{
	if (_EquippedWeapon->GetMaxAmmo() == _EquippedWeapon->GetAmmo())return;
	if (CarriedAmmo > 0&&CombatState!=ECombatState::ECS_Reloading)
	{
		ServerReload();
	}
	
}

void UCombatComponent::ServerReload_Implementation()
{
	if (CombatCharacter == nullptr||_EquippedWeapon==nullptr)return;

	

	CombatState = ECombatState::ECS_Reloading;
	HandleReload();
}

void UCombatComponent::FinishedReload()
{
	if (CombatCharacter == nullptr)return;
	if (CombatCharacter->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValue();
	}
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::UpdateAmmoValue()
{
	if (CombatCharacter == nullptr || _EquippedWeapon == nullptr)return;
	int32 ReloadAmount = AmountToReload();


	if (CarriedAmmoMap.Contains(_EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[_EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[_EquippedWeapon->GetWeaponType()];

	}

	PlayerController = PlayerController == nullptr ? Cast<ABlaster_PlayerController>(CombatCharacter->Controller) : PlayerController;
	if (PlayerController)
	{
		PlayerController->Set_HUD_CarriedAmmo(CarriedAmmo);
	}


	_EquippedWeapon->AddAmmo(-ReloadAmount);

}
void UCombatComponent::HandleReload()
{

	CombatCharacter->PlayReloadMontage();
}


void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;

	}
}




void UCombatComponent::InterpFOV(float Deltatime)
{
	if (_EquippedWeapon == nullptr)return ;

	if (bAiming)
	{
		if (CombatCharacter->bIsCrouched)
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV,_EquippedWeapon->GetZoomFOV()+15.f, Deltatime, _EquippedWeapon->GetZoomInterpSpeed());
		}
		else
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV,_EquippedWeapon->GetZoomFOV(),Deltatime,_EquippedWeapon->GetZoomInterpSpeed());
		}

	}

	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV,DefaultFOV,Deltatime,ZoomInterpSpeed);
	}


	if (CombatCharacter && CombatCharacter->GetPlayerCamera())
	{
		CombatCharacter->GetPlayerCamera()->SetFieldOfView(CurrentFOV);
	}
}


void UCombatComponent::SetHudCrossHair(float DeltaTime)
{
	if (CombatCharacter == nullptr||CombatCharacter->Controller==nullptr)return;

	PlayerController = PlayerController == nullptr ? Cast<ABlaster_PlayerController>(CombatCharacter->Controller) : PlayerController;

	if (PlayerController)
	{
		PlayerHUD = PlayerHUD == nullptr ? Cast<ABlaster_HUD>(PlayerController->GetHUD()) : PlayerHUD;
	
		if (PlayerHUD)
		{
			
			if (_EquippedWeapon)
			{
				HUDPackage.CrossHairCenter = _EquippedWeapon->CrossHairCenter;
				HUDPackage.CrossHairLeft = _EquippedWeapon->CrossHairLeft;
				HUDPackage.CrossHairRight = _EquippedWeapon->CrossHairRight;
				HUDPackage.CrossHairTop = _EquippedWeapon->CrossHairTop;
				HUDPackage.CrossHairBottom = _EquippedWeapon->CrossHairBottom;

			
			}

			else
			{
				HUDPackage.CrossHairCenter = nullptr;
				HUDPackage.CrossHairLeft = nullptr;
				HUDPackage.CrossHairRight = nullptr;
				HUDPackage.CrossHairTop = nullptr;
				HUDPackage.CrossHairBottom = nullptr;

			}

			FVector2D WalkSpeedRange(0.f, CombatCharacter->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f,1.f);

			FVector Velocity = CombatCharacter->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor=FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,Velocity.Size());

			if (CombatCharacter->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,2.25f,DeltaTime,2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor,0.58f,DeltaTime,30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor,0.f,DeltaTime,6.f);

			HUDPackage.CrossHairSpread 
			=	0.5f+
				CrosshairVelocityFactor+
				CrosshairInAirFactor-
				CrosshairAimFactor+
				CrosshairShootingFactor;

			PlayerHUD->SetHUDPackage(HUDPackage);

		}
	
	}

}

int32 UCombatComponent::AmountToReload()
{
	if (_EquippedWeapon == nullptr)return 0;
	int32 RoomInAmmo = _EquippedWeapon->GetMaxAmmo()- _EquippedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(_EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[_EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInAmmo,AmountCarried);
		return FMath::Clamp(RoomInAmmo,0,Least);
	}

	return int32();
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);	
	
	if (CombatCharacter)
	{
		CombatCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}

}


void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	
	if (CombatCharacter)
	{
		CombatCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}












