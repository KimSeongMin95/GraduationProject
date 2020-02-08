// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHUD.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "PioneerManager.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

AMyHUD::AMyHUD()
{
	MinimapSize = 270.0f;
	MinimapUI = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("MinimapUI"));
	ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> MinimapUITexObj(TEXT("TextureRenderTarget2D'/Game/UI/Minimap.Minimap'"));
	if (MinimapUITexObj.Succeeded())
	{
		MinimapUI = MinimapUITexObj.Object;

		MinimapUI->InitAutoFormat(MinimapSize, MinimapSize); // 크기 설정
		// MinimapUI->UpdateResourceImmediate(true); // 필요 없는 듯.
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> MainUITexObj(TEXT("Texture2D'/Game/UI/MainUI.MainUI'"));
	if (MainUITexObj.Succeeded())
	{
		MainUI = MainUITexObj.Object;
	}



	static ConstructorHelpers::FObjectFinder<UTexture2D> IconPioneerUITexObj(TEXT("Texture2D'/Game/UI/Icon_Pioneer2.Icon_Pioneer2'"));
	if (IconPioneerUITexObj.Succeeded())
	{
		IconPioneerUI = IconPioneerUITexObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconMineralUITexObj(TEXT("Texture2D'/Game/UI/Icon_Mineral.Icon_Mineral'"));
	if (IconMineralUITexObj.Succeeded())
	{
		IconMineralUI = IconMineralUITexObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconOrganicUITexObj(TEXT("Texture2D'/Game/UI/Icon_Organic.Icon_Organic'"));
	if (IconOrganicUITexObj.Succeeded())
	{
		IconOrganicUI = IconOrganicUITexObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconEnergyUITexObj(TEXT("Texture2D'/Game/UI/Icon_Energy.Icon_Energy'"));
	if (IconEnergyUITexObj.Succeeded())
	{
		IconEnergyUI = IconEnergyUITexObj.Object;
	}
}

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!PioneerManager)
	{
		UWorld* const world = GetWorld();
		if (!world)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
			return;
		}

		// UWorld에서 AWorldViewCameraActor를 찾습니다.
		if (PioneerManager == nullptr)
		{
			for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
			{
				PioneerManager = *ActorItr;
			}
		}
	}

	
	
		//PioneerText.FromInt(PioneerManager->Pioneers.Num());
	


	const float ViewportSizeX = GEngine->GameViewport->Viewport->GetSizeXY().X;
	const float ViewportSizeY = GEngine->GameViewport->Viewport->GetSizeXY().Y;
	FVector2D ViewportSize = FVector2D(ViewportSizeX, ViewportSizeY);





	FVector2D MinimapUIResize(MinimapSize * (ViewportSizeX / 1920.0f), MinimapSize * (ViewportSizeY / 1080.0f));
	MinimapUI->ResizeTarget(MinimapUIResize.X, MinimapUIResize.Y);
	FVector2D MinimapUIDrawPosition(25.0f * (ViewportSizeX / 1920.0f), ViewportSizeY - MinimapUIResize.Y);

	// draw the Tex
	FCanvasTileItem MinimapUITileItem(MinimapUIDrawPosition, MinimapUI->Resource, MinimapUIResize, FLinearColor::White);
	MinimapUITileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(MinimapUITileItem);


	const FVector2D MainUIDrawPosition(0.0f, 0.0f);

	// draw the Tex
	FCanvasTileItem MainUITileItem(MainUIDrawPosition, MainUI->Resource, ViewportSize, FLinearColor::White);
	MainUITileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(MainUITileItem);

	const FVector2D IconDrawPisition(1600.0f * (ViewportSizeX / 1920.0f), 850.0f * (ViewportSizeY / 1080.0f));
	const FVector2D IconDrawSize(45.0f * (ViewportSizeX / 1920.0f), 45.0f* (ViewportSizeY / 1080.0f));

	FCanvasTileItem IconPioneerUITileItem(IconDrawPisition + FVector2D(0.0f, 0.0f * (ViewportSizeY / 1080.0f)), IconPioneerUI->Resource, IconDrawSize, FLinearColor::White);
	IconPioneerUITileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(IconPioneerUITileItem);

	// 서버 추가로 인한 TEXT 매크로 재정의 문제 때문에 주석처리 했습니다.

	if (PioneerManager)
		DrawText(FString::FromInt(PioneerManager->Pioneers.Num()), FLinearColor::Green, IconDrawPisition.X + 55.0f, IconDrawPisition.Y, IconPioneerFont, 1.5f * (ViewportSizeX / 1920.0f), false);

	FCanvasTileItem IconMineralUITileItem(IconDrawPisition + FVector2D(0.0f, 50.0f * (ViewportSizeY / 1080.0f)), IconMineralUI->Resource, IconDrawSize, FLinearColor::White);
	IconMineralUITileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(IconMineralUITileItem);

	DrawText(TEXT("312"), FLinearColor::Green, IconDrawPisition.X + 55.0f, IconDrawPisition.Y + 50.0f, IconMineralFont, 1.5f * (ViewportSizeX / 1920.0f), false);

	FCanvasTileItem IconOrganicUITileItem(IconDrawPisition + FVector2D(0.0f, 100.0f * (ViewportSizeY / 1080.0f)), IconOrganicUI->Resource, IconDrawSize, FLinearColor::White);
	IconOrganicUITileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(IconOrganicUITileItem);

	DrawText(TEXT("178"), FLinearColor::Green, IconDrawPisition.X + 55.0f, IconDrawPisition.Y + 100.0f, IconOrganicFont, 1.5f * (ViewportSizeX / 1920.0f), false);

	FCanvasTileItem IconEnergyUITileItem(IconDrawPisition + FVector2D(0.0f, 150.0f * (ViewportSizeY / 1080.0f)), IconEnergyUI->Resource, IconDrawSize, FLinearColor::White);
	IconEnergyUITileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(IconEnergyUITileItem);

	DrawText(TEXT("50"), FLinearColor::Green, IconDrawPisition.X + 55.0f, IconDrawPisition.Y + 150.0f, IconEnergyFont, 1.5f * (ViewportSizeX / 1920.0f), false);

}