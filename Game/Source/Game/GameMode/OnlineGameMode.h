// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** �𸮾��� ��� ���� : Start ***/
#include "UObject/ConstructorHelpers.h"

#include "Engine.h"

#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** �𸮾��� ��� ���� : End ***/


#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OnlineGameMode.generated.h"

UCLASS()
class GAME_API AOnlineGameMode : public AGameModeBase
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AOnlineGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()���� ����˴ϴ�. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** AMainScreenGameMode : Start ***/
private:
	class cClientSocket* ClientSocket = nullptr;

	class cServerSocketInGame* ServerSocketInGame = nullptr;
	class cClientSocketInGame* ClientSocketInGame = nullptr;


	//UPROPERTY(VisibleAnywhere, Category = "Widget")
	//	/** ���� UI */
	//	class UInGameWidget* InGameWidget = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "AOnlineGameMode")
		class APioneerController* PioneerController = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AOnlineGameMode")
		class APioneerManager* PioneerManager = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AOnlineGameMode")
		class ASpaceShip* SpaceShip = nullptr;

protected:
	float temp = 0.0f;

public:


private:
	void FindPioneerController();

	void SpawnPioneerManager(); 
	void SpawnSpaceShip(class ASpaceShip** pSpaceShip, FTransform Transform);
	

protected:



public:
	
	

	static const float CellSize;
/*** AMainScreenGameMode : End ***/
};
