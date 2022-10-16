// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Rocket_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ARocket_Projectile : public AProjectile
{
	GENERATED_BODY()
	

public:
	
	ARocket_Projectile();

	virtual void Destroyed()override;
protected:

	virtual void OnHit
		(
			UPrimitiveComponent* HitComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			FVector NormalImpulse,
			const FHitResult& Hit
		)override;

	virtual void BeginPlay()override;
	
	void DestroyTimerFinished();

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* TrailSystem;
	
	UPROPERTY()
		class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(EditAnywhere)
		USoundCue* ProjectileLoop;

	UPROPERTY()
		UAudioComponent* ProjectileLoopComponent;
	
	UPROPERTY(EditAnywhere)
		USoundAttenuation* LoopingSoundAtten;

	UPROPERTY(VisibleAnywhere)
		class URocketMovementComponent* RocketMovementComponent;

private:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* RocketMesh;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
		float DestroyTime = 3.f;
};
