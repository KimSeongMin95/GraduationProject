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

	/*class UTexture2D* SubUI_1;
	class UTexture2D* SubUI_2;
	class UTexture2D* SubUI_3;
	class UTexture2D* SubUI_4;
	class UTexture2D* SubUI_5;*/
};
