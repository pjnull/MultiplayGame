// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/Blaster_PlayerController.h"
#include "Net/UnrealNetwork.h" 

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);


}



void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore()+ScoreAmount);

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlaster_PlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->Set_HUD_Score(GetScore());
		}
	}
}

void ABlasterPlayerState::OnRep_Score()
{

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlaster_PlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->Set_HUD_Score(GetScore());
		}
	}

}


void ABlasterPlayerState::AddToDeath(int32 Defeat)
{
	Defeats += Defeat;

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlaster_PlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->Set_HUD_Death(Defeats);
		}
	}
}



void ABlasterPlayerState::OnRep_Death()
{
	
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlaster_PlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->Set_HUD_Score(Score);
		}
	}
}
