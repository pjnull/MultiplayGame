// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* NewPlayer)override;
	virtual void Logout(AController* Exiting)override;
	
private:
	
	void InGame();
	
	FTimerHandle InGameTimer;



	UPROPERTY(EditDefaultsOnly)
		float InGameDelay = 10.f;

	int32 CountDown = 10;
};
