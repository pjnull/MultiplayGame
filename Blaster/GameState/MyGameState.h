// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AMyGameState : public AGameState
{
	GENERATED_BODY()


public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps)const override;
	void UpdateTopScore(class ABlasterPlayerState* ScorePlayer);
	
	UPROPERTY(Replicated)
	TArray<class ABlasterPlayerState*>TopScoringPlayers;

	float TopScore = 0.f;

};
