// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blaster_HUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	class UTexture2D* CrossHairCenter;
	class UTexture2D* CrossHairLeft;
	class UTexture2D* CrossHairRight;
	class UTexture2D* CrossHairTop;
	class UTexture2D* CrossHairBottom;

	float CrossHairSpread;
	FLinearColor CrosshairColor;
};


UCLASS()
class BLASTER_API ABlaster_HUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere,Category="Player Stat")
		TSubclassOf<class UUserWidget> CharacterOverlayClass;
	
	UPROPERTY(EditAnywhere, Category = "Announce")
		TSubclassOf<class UUserWidget> AnnounceClass;


	UPROPERTY()
		class UCharacterOverlay* CharacterOverlay;

	UPROPERTY()
		class UAnnouncement* Announcement;

	void AddCharacterOverlay();
	void AddAnnouncement();


protected:

	virtual void BeginPlay()override;


private:
	FHUDPackage HUDPackage;

	void DrawCrossHair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);
	
	UPROPERTY(EditAnywhere)
		float CrosshairSpreadMax = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; };
};
