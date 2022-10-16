// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blaster_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlaster_PlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	void Set_HUD_Hp(float Hp,float MaxHp);
	void Set_HUD_Score(float Score);
	void Set_HUD_Death(int32 Defeats);
	void Set_HUD_WeaponAmmo(int32 Ammo);
	void Set_HUD_WeaponAmmo();
	void Set_HUD_CarriedAmmo(int32 Ammo);
	void Set_HUD_CarriedAmmo();
	void Set_HUD_MatchCountDown(float CountDown);
	void Set_HUD_AnnounceCountDown(float CountDown);


	void OnMatchStateSet(FName State);

	void HandleMatchHasStarted();
	void HandleCooldown();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime)override;
	
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;


protected:
	virtual void BeginPlay()override;
	
	
	void SetHUDTime();
	void PollInit();


	UFUNCTION(Server,Reliable)
		void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Server, Reliable)
		void ServerCheckMatchState();

	
	UFUNCTION(Client, Reliable)
		void ClientReportServerTime(float TimeOfClientRequest,float TimeServerReceivedClientRequest);


	UFUNCTION(Client, Reliable)
		void ClientJoinMidgame(FName StateOfMatch,float	Warmup,float Match, float Cooldown, float StartingTime);

	float ClientServerDelta = 0.f;

	UPROPERTY(EditAnywhere,Category=Time)
		float TimeSyncFrequency = 5.f;

	void CheckTimeSync(float DeltaTime);


	float TimeSyncRunningTime = 0.f;



private:

	UPROPERTY()
		class ABlaster_HUD* Blaster_HUD;

	UPROPERTY()
		class ABlasterGameMode* BlasterGameMode;

	float MatchTime = 0.f;
	float WarmUpTime = 0.f;
	float LevelStartTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountDownInt=0;



	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
		FName MatchState;

	UFUNCTION()
		void OnRep_MatchState();

	UPROPERTY()
		class UCharacterOverlay* CharacterOverlay;

	bool bInitializeCharacterOverlay = false;

	float HUDHp;
	float HUDMaxHp;
	float HUDScore;
	int32 HUDDefeats;
};
