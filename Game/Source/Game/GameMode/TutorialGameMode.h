// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/

/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialGameMode.generated.h"


UCLASS()
class GAME_API ATutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	ATutorialGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()���� ����˴ϴ�. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** ATutorialGameMode : Start ***/
private:
	class APioneerManager* PioneerManager = nullptr;

private:
	/** APioneerManager ��ü�� �����մϴ�. */
	void SpawnPioneerManager(); 
/*** ATutorialGameMode : End ***/
};
