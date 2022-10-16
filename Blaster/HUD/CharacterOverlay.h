// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* Hp_Bar;

	UPROPERTY(meta=(BindWidget))
		class UTextBlock* Hp_Text;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Kill_Amount;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Death_Amount;
	
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WeaponAmmo_Amount;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CarriedAmmo_Amount;
	
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MatchCountDown;

};
