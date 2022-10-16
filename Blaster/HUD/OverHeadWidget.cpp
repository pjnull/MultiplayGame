// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverHeadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel,InWorld);


}


void UOverHeadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverHeadWidget::GetPlayerName()
{
	APawn* PlayerPawn = GetOwningPlayerPawn();

	APlayerState* PlayerState = PlayerPawn->GetPlayerState();

	FString PlayerName = PlayerState->GetPlayerName();
}

void UOverHeadWidget::ShowPlayNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	
	FString	Role;
	switch (RemoteRole)
	{
		case ENetRole::ROLE_Authority:
			Role = FString("Authority");
			break;
		case ENetRole::ROLE_AutonomousProxy:
			Role = FString("Autonomous Proxy");
			break;
		case ENetRole::ROLE_SimulatedProxy:
			Role = FString("Simulated Proxy");
			break;
		case ENetRole::ROLE_None:
			Role = FString("None");
			break;
		
	}
	
	FString PlayerName = "";

	if (InPawn)
	{
		APlayerState* PlayerState = InPawn->GetPlayerState();
		if (PlayerState)
		{
			PlayerName = PlayerState->GetPlayerName();
		}
	}

	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s\n%s"),*Role,*PlayerName);
	SetDisplayText(RemoteRoleString);
}