// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHUD.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

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

	/*static ConstructorHelpers::FObjectFinder<UTexture2D> SubUITexObj(TEXT("Texture2D'/Game/UI/Black.Black'"));
	if (SubUITexObj.Succeeded())
	{
		SubUI_1 = SubUITexObj.Object;
		SubUI_2 = SubUITexObj.Object;
		SubUI_3 = SubUITexObj.Object;
		SubUI_4 = SubUITexObj.Object;
		SubUI_5 = SubUITexObj.Object;
	}*/
}

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();

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








	/*FCanvasTileItem SubUI_1TileItem(FVector2D(), SubUI_1->Resource, FVector2D(), FLinearColor::White);
	SubUI_1TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(SubUI_1TileItem);

	FCanvasTileItem SubUI_2TileItem(FVector2D(), SubUI_2->Resource, FVector2D(), FLinearColor::White);
	SubUI_2TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(SubUI_2TileItem);

	FCanvasTileItem SubUI_3TileItem(FVector2D(), SubUI_3->Resource, FVector2D(), FLinearColor::White);
	SubUI_3TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(SubUI_3TileItem);

	FCanvasTileItem SubUI_4TileItem(FVector2D(), SubUI_4->Resource, FVector2D(), FLinearColor::White);
	SubUI_4TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(SubUI_4TileItem);

	FCanvasTileItem SubUI_5TileItem(FVector2D(), SubUI_5->Resource, FVector2D(), FLinearColor::White);
	SubUI_5TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(SubUI_5TileItem);*/

	

	const FVector2D MainUIDrawPosition(0.0f, 0.0f);

	// draw the Tex
	FCanvasTileItem MainUITileItem(MainUIDrawPosition, MainUI->Resource, ViewportSize, FLinearColor::White);
	MainUITileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(MainUITileItem);
}