// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHUD.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/

AMyHUD::AMyHUD()
{
	//// Set the crosshair texture
	//static ConstructorHelpers::FObjectFinder<UTexture2D> TexObj(TEXT("/Game/FirstPerson/Textures/TexObj"));
	//Tex = TexObj.Object;
}

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();

	//// Draw very simple crosshair
	//// find center of the Canvas
	//const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	//// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	//const FVector2D DrawPosition((Center.X), (Center.Y + 20.0f));
	//// draw the Tex
	//FCanvasTileItem TileItem(DrawPosition, Tex->Resource, FLinearColor::White);
	//TileItem.BlendMode = SE_BLEND_Translucent;
	//Canvas->DrawItem(TileItem);
}