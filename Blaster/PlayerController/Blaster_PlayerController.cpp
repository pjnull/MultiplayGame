// Fill out your copyright notice in the Description page of Project Settings.


#include "Blaster_PlayerController.h"
#include "Blaster/HUD/Blaster_HUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterComponent/CombatComponent.h"
#include "Blaster/GameState/MyGameState.h"
#include "Kismet/GameplayStatics.h"


void ABlaster_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	Blaster_HUD =Cast<ABlaster_HUD>(GetHUD());
	ServerCheckMatchState();
}


void ABlaster_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlaster_PlayerController, MatchState);
}




void ABlaster_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetHUDTime();

	CheckTimeSync(DeltaTime);

	PollInit();

}



void ABlaster_PlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}


void ABlaster_PlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* Gamemode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));

	if (Gamemode)
	{
		WarmUpTime = Gamemode->WarmupTime;
		MatchTime = Gamemode->MatchTime;
		LevelStartTime = Gamemode->LevelStartTime;
		CooldownTime = Gamemode->CooldownTime;
		MatchState = Gamemode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmUpTime, MatchTime, CooldownTime, LevelStartTime);
		
		

	}
}


void ABlaster_PlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float	Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmUpTime = Warmup;
	MatchTime = Match;
	LevelStartTime = StartingTime;
	CooldownTime = Cooldown;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (Blaster_HUD && MatchState == MatchState::WaitingToStart)
	{
		Blaster_HUD->AddAnnouncement();
	}

}



void ABlaster_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);

	if (BlasterCharacter)
	{
		Set_HUD_Hp(BlasterCharacter->GetHp(), BlasterCharacter->GetMaxHp());
	}
}





void ABlaster_PlayerController::Set_HUD_Hp(float Hp, float MaxHp)
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;

	bool bHudVaild =
		Blaster_HUD &&
		Blaster_HUD->CharacterOverlay &&
		Blaster_HUD->CharacterOverlay->Hp_Bar &&
		Blaster_HUD->CharacterOverlay->Hp_Text;
	
	
	if(bHudVaild)
	{
		const float HpPer = Hp / MaxHp;
		Blaster_HUD->CharacterOverlay->Hp_Bar->SetPercent(HpPer);
		FString HpText = FString::Printf(TEXT("%d/%d"),FMath::CeilToInt(Hp), FMath::CeilToInt(MaxHp));
	
		Blaster_HUD->CharacterOverlay->Hp_Text->SetText(FText::FromString(HpText));

	
	}

	else
	{
		bInitializeCharacterOverlay = true;
		HUDHp = Hp;
		HUDMaxHp = MaxHp;
	}
}

void ABlaster_PlayerController::Set_HUD_Score(float Score)
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;
	
	bool bHudVaild =
		Blaster_HUD &&
		Blaster_HUD->CharacterOverlay &&
		Blaster_HUD->CharacterOverlay->Kill_Amount;


	if (bHudVaild)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));

		Blaster_HUD->CharacterOverlay->Kill_Amount->SetText(FText::FromString(ScoreText));


	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}

}

void ABlaster_PlayerController::Set_HUD_Death(int32 Defeats)
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;
	bool bHudVaild =
		Blaster_HUD &&
		Blaster_HUD->CharacterOverlay &&
		Blaster_HUD->CharacterOverlay->Death_Amount;


	if (bHudVaild)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), Defeats);

		Blaster_HUD->CharacterOverlay->Death_Amount->SetText(FText::FromString(ScoreText));


	}

	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}

}

void ABlaster_PlayerController::Set_HUD_WeaponAmmo(int32 Ammo)
{
	

	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;
	bool bHudVaild =
		Blaster_HUD &&
		Blaster_HUD->CharacterOverlay &&
		Blaster_HUD->CharacterOverlay->WeaponAmmo_Amount;


	if (bHudVaild)
	{
		FString AmmoText = FString::Printf(TEXT("%d /"), Ammo);

		Blaster_HUD->CharacterOverlay->WeaponAmmo_Amount->SetText(FText::FromString(AmmoText));


	}
	
}

void ABlaster_PlayerController::Set_HUD_WeaponAmmo()
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;
	bool bHudVaild =
		Blaster_HUD &&
		Blaster_HUD->CharacterOverlay &&
		Blaster_HUD->CharacterOverlay->WeaponAmmo_Amount;


	if (bHudVaild)
	{
		Blaster_HUD->CharacterOverlay->WeaponAmmo_Amount->SetText(FText::FromString(" "));


	}
}

void ABlaster_PlayerController::Set_HUD_CarriedAmmo(int32 Ammo)
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;
	bool bHudVaild =
		Blaster_HUD &&
		Blaster_HUD->CharacterOverlay &&
		Blaster_HUD->CharacterOverlay->CarriedAmmo_Amount;


	if (bHudVaild)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);

		Blaster_HUD->CharacterOverlay->CarriedAmmo_Amount->SetText(FText::FromString(AmmoText));


	}
}

void ABlaster_PlayerController::Set_HUD_CarriedAmmo()
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;
	bool bHudVaild =
		Blaster_HUD &&
		Blaster_HUD->CharacterOverlay &&
		Blaster_HUD->CharacterOverlay->CarriedAmmo_Amount;


	if (bHudVaild)
	{
		Blaster_HUD->CharacterOverlay->CarriedAmmo_Amount->SetText(FText::FromString(" "));

	}
}

void ABlaster_PlayerController::Set_HUD_MatchCountDown(float CountDown)
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;
	bool bHudVaild=
		Blaster_HUD &&
		Blaster_HUD->CharacterOverlay &&
		Blaster_HUD->CharacterOverlay->MatchCountDown;

	if (bHudVaild)
	{
		if (CountDown<0.f)
		{
			Blaster_HUD->CharacterOverlay->MatchCountDown->SetText(FText());
			return;
		}
		int32 Min = FMath::FloorToInt(CountDown/60.f);
		int32 Sec = CountDown-Min*60;

		FString CountDownString= FString::Printf(TEXT("%02d : %02d"), Min,Sec);
		Blaster_HUD->CharacterOverlay->MatchCountDown->SetText(FText::FromString(CountDownString));

	}
}

void ABlaster_PlayerController::Set_HUD_AnnounceCountDown(float CountDown)
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;
	bool bHudVaild =
		Blaster_HUD &&
		Blaster_HUD->Announcement &&
		Blaster_HUD->Announcement->WarmUpTime;

	if (bHudVaild)
	{
		if (CountDown < 0.f)
		{
			Blaster_HUD->Announcement->WarmUpTime->SetText(FText());
			return;
		}

		int32 Min = FMath::FloorToInt(CountDown / 60.f);
		int32 Sec = CountDown - Min * 60;

		FString CountDownString = FString::Printf(TEXT("%02d : %02d"), Min, Sec);
		Blaster_HUD->Announcement->WarmUpTime->SetText(FText::FromString(CountDownString));

	}
}


void ABlaster_PlayerController::SetHUDTime()
{
	float Timeleft = 0.f;
	if (MatchState == MatchState::WaitingToStart)Timeleft = WarmUpTime - GetServerTime() + LevelStartTime;
	else if (MatchState == MatchState::InProgress)Timeleft = WarmUpTime + MatchTime - GetServerTime() + LevelStartTime;
	else if (MatchState == MatchState::Cooldown)Timeleft = CooldownTime+WarmUpTime + MatchTime - GetServerTime() + LevelStartTime;


	uint32 SecLeft = FMath::CeilToInt(Timeleft);

	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		
		if (BlasterGameMode)
		{
			SecLeft= FMath::CeilToInt(BlasterGameMode->GetCountdownTime()+LevelStartTime);
		}
	}

	if (CountDownInt != SecLeft)
	{
		if (MatchState == MatchState::WaitingToStart||MatchState==MatchState::Cooldown)
		{
			Set_HUD_AnnounceCountDown(Timeleft);
		}
		if (MatchState == MatchState::InProgress)
		{
			Set_HUD_MatchCountDown(Timeleft);
		}
	}
	CountDownInt = SecLeft;
}





void ABlaster_PlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (Blaster_HUD && Blaster_HUD->CharacterOverlay)
		{
			CharacterOverlay = Blaster_HUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				Set_HUD_Hp(HUDHp, HUDMaxHp);
				Set_HUD_Score(HUDScore);
				Set_HUD_Death(HUDDefeats);
			}
		}
	}
}




	

void ABlaster_PlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest,ServerTimeOfReceipt);
}

void ABlaster_PlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RTT = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f *RTT);
	
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();

}


float ABlaster_PlayerController::GetServerTime()
{
	if (HasAuthority())return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlaster_PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());

	}
}

void ABlaster_PlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	
	else if (MatchState==MatchState::Cooldown)
	{
		HandleCooldown();
	}
}




void ABlaster_PlayerController::OnRep_MatchState()
{

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}


void ABlaster_PlayerController::HandleMatchHasStarted()
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;

	if (Blaster_HUD)
	{
		Blaster_HUD->AddCharacterOverlay();

		if (Blaster_HUD->Announcement)
		{
			Blaster_HUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlaster_PlayerController::HandleCooldown()
{
	Blaster_HUD = Blaster_HUD == nullptr ? Cast<ABlaster_HUD>(GetHUD()) : Blaster_HUD;

	if (Blaster_HUD)
	{
		Blaster_HUD->CharacterOverlay->RemoveFromParent();

		bool bHudVaild = Blaster_HUD->Announcement && 
						 Blaster_HUD->Announcement->AnnouncementText && 
						 Blaster_HUD->Announcement->InfoText;
		
		if (bHudVaild)
		{
			Blaster_HUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Start In:");
			Blaster_HUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
			
			AMyGameState* BlasterGameSate=Cast<AMyGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			
			if (BlasterGameSate&&BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*>TopPlayer=BlasterGameSate->TopScoringPlayers;
				
				FString InfoTextString;

				if (TopPlayer.Num() == 0)
				{
					InfoTextString = FString("There is no winner");
				}
				else if (TopPlayer.Num() == 1 && TopPlayer[0] == BlasterPlayerState)
				{
					InfoTextString = FString("You Are The winner");
				}
				else if (TopPlayer.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("The Winner: \n %s"),*TopPlayer[0]->GetPlayerName());
				}
				else if (TopPlayer.Num() >1)
				{
					InfoTextString = FString("The winner:\n");
					for (auto Players:TopPlayer)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *Players->GetPlayerName()));

					}
				}
				Blaster_HUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
			
			
		}
	}

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());

	if (BlasterCharacter&&BlasterCharacter->GetCombat())
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}
