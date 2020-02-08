// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Engine.h"
#include "Engine/TextureRenderTarget2D.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

UCLASS()
class GAME_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMyHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	class UTextureRenderTarget2D* MinimapUI;
	float MinimapSize;

	class UTexture2D* MainUI;

	class UTexture2D* IconPioneerUI;
	class UTexture2D* IconMineralUI;
	class UTexture2D* IconOrganicUI;
	class UTexture2D* IconEnergyUI;

public:
	class UFont* IconPioneerFont;
	class UFont* IconMineralFont;
	class UFont* IconOrganicFont;
	class UFont* IconEnergyFont;

	class APioneerManager* PioneerManager = nullptr;
	const FString PioneerText;
};
