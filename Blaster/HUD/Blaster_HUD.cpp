// Fill out your copyright notice in the Description page of Project Settings.


#include "Blaster_HUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"

void ABlaster_HUD::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ABlaster_HUD::AddCharacterOverlay()
{
	APlayerController* PlayerController=GetOwningPlayerController();

	if (PlayerController&&CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController,CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	
	}
}

void ABlaster_HUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && AnnounceClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnounceClass);
		Announcement->AddToViewport();

	}
}



void ABlaster_HUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;

	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X/2.f,ViewportSize.Y/2.f);
		
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrossHairSpread;

		if (HUDPackage.CrossHairCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrossHair(HUDPackage.CrossHairCenter,ViewportCenter,Spread, HUDPackage.CrosshairColor);
		}
		if(HUDPackage.CrossHairLeft)
		{

			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrossHair(HUDPackage.CrossHairLeft, ViewportCenter,Spread, HUDPackage.CrosshairColor);
		}
		if(HUDPackage.CrossHairRight)
		{

			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrossHair(HUDPackage.CrossHairRight, ViewportCenter,Spread, HUDPackage.CrosshairColor);
		}
		if(HUDPackage.CrossHairTop)
		{

			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrossHair(HUDPackage.CrossHairTop, ViewportCenter,Spread, HUDPackage.CrosshairColor);
		}
		if(HUDPackage.CrossHairBottom)
		{

			FVector2D Spread(0.f, SpreadScaled);
			DrawCrossHair(HUDPackage.CrossHairBottom, ViewportCenter,Spread, HUDPackage.CrosshairColor);
		}
	}
}


void ABlaster_HUD::DrawCrossHair(UTexture2D* Texture, FVector2D ViewportCenter,FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	const FVector2D TextureDrawPoint
	(
		ViewportCenter.X - (TextureWidth / 2.f)+Spread.X,
		ViewportCenter.Y-(TextureHeight/2.f)+Spread.Y
	);

	DrawTexture
			(
				Texture,TextureDrawPoint.X, TextureDrawPoint.Y,
				TextureWidth,TextureHeight,
				0.f,0.f,
				1.f,1.f,
				CrosshairColor
			);
}