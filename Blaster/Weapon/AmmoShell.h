// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoShell.generated.h"

UCLASS()
class BLASTER_API AAmmoShell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoShell();

	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnHit
		(
			UPrimitiveComponent* HitComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			FVector NormalImpulse,
			const FHitResult& Hit
		);




private:
	
	
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* AmmoShell;
	


	UPROPERTY(EditAnywhere)
		float ShellEjectionImpulse;

	UPROPERTY(EditAnywhere)
		class USoundCue* ShellSound;
};
