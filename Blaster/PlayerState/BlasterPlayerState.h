// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	
	UFUNCTION()
		virtual void OnRep_Death();
	
	void AddToScore(float Score);
	void AddToDeath(int32 Defeats);
private:
	
	UPROPERTY()
		class ABlasterCharacter* Character;
	UPROPERTY()
		class ABlaster_PlayerController* Controller;
	
	UPROPERTY(ReplicatedUsing=OnRep_Death)
		int32 Defeats;
};
