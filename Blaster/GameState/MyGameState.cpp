// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameState,TopScoringPlayers);

}

void AMyGameState::UpdateTopScore(ABlasterPlayerState* ScorePlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScorePlayer);
		TopScore = ScorePlayer->GetScore();
	}
	else if(ScorePlayer->GetScore()==TopScore)
	{
		TopScoringPlayers.AddUnique(ScorePlayer);
	}
	else if (ScorePlayer->GetScore()>TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScorePlayer);
		TopScore = ScorePlayer->GetScore();
	}

}