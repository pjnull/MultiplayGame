// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterType/CombatState.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr)return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	IsAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccel = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size()>0.f?true:false;
	
	bWeaponEquipped		= BlasterCharacter->IsWeaponEquipped();
	bIsCrouch			= BlasterCharacter->bIsCrouched;
	bAiming				= BlasterCharacter->IsAiming();
	EquippedWeapon		= BlasterCharacter->GetEquippedWeapon();
	bRotateRootBone		= BlasterCharacter->ShouldRotateRootBone();
	bElimmed			= BlasterCharacter->IsElimmed();

	TurningInPlace = BlasterCharacter->GetTurningInPlace();
	
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation,DeltaRot,DeltaTime,6.f);
	YawOffset = DeltaRotation.Yaw;
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation,CharacterRotationLastFrame);

	const float Target = Delta.Yaw/DeltaTime;
	const float Interp = FMath::FInterpTo(Lean,Target,DeltaTime,6.f);
	Lean = FMath::Clamp(Interp,-90.f,90.f);

	AO_Yaw = BlasterCharacter->GetAOYaw();
	AO_Pitch = BlasterCharacter->GetAOPitch();

	if (bWeaponEquipped&&EquippedWeapon&&EquippedWeapon->GetWeaponMesh()&&BlasterCharacter->GetMesh())
	{
		
		LeftHandTrans = 
		EquippedWeapon->GetWeaponMesh()->GetSocketTransform
		(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		
		FVector	 OutPosition;
		FRotator OutRotation;

		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"),LeftHandTrans.GetLocation(),FRotator::ZeroRotator,OutPosition,OutRotation);

		LeftHandTrans.SetLocation(OutPosition);
		LeftHandTrans.SetRotation(FQuat(OutRotation));

	

		
		if (BlasterCharacter->IsLocallyControlled())
		{
			bLocallyControllered = true;
			
			FTransform RightHandTrans =
			EquippedWeapon->GetWeaponMesh()->GetSocketTransform
			(FName("hand_r"), ERelativeTransformSpace::RTS_World);

			FTransform MuzzleTipTransform 
			= EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
			
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
			
			FRotator LookAtRotation= UKismetMathLibrary::FindLookAtRotation(RightHandTrans.GetLocation(), RightHandTrans.GetLocation() + (RightHandTrans.GetLocation() - BlasterCharacter->GetHitTarget()));
			
			RightHandRotation = FMath::RInterpTo(RightHandRotation,LookAtRotation,DeltaTime,30.f);
			
			
			DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation()+MuzzleX*1000.f,FColor::Red);
			
			DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation(),BlasterCharacter->GetHitTarget(),FColor::Orange);
		

				
		}
		
		
	
	}

	bUseFABRIK = BlasterCharacter->GetCombatState()!=ECombatState::ECS_Reloading;
	bUseAimOffset = BlasterCharacter->GetCombatState()!=ECombatState::ECS_Reloading&&!BlasterCharacter->GetDisableGameplay();
	bTransformRightHand = BlasterCharacter->GetCombatState()!=ECombatState::ECS_Reloading && !BlasterCharacter->GetDisableGameplay();
	
}
