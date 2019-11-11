// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
/*** �𸮾��� ��� ���� : End ***/

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
	/**    */
	class UTexture2D* Tex;
};
