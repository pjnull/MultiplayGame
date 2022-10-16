// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
		class UTextBlock* DisplayText;
	UFUNCTION(BlueprintCallable)
		void ShowPlayNetRole(APawn* InPawn);
	void SetDisplayText(FString TextToDisplay);
	
	UFUNCTION(BlueprintCallable)
		void GetPlayerName();

protected:
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel,UWorld* InWorld)override;
};
