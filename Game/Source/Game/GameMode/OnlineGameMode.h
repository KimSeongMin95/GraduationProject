// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "UObject/ConstructorHelpers.h"

#include "Engine.h"

#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/


#include "CoreMinimal.h"
#include "GameMode/InGameMode.h"
#include "OnlineGameMode.generated.h"


UENUM()
enum class EOnlineGameState : uint8
{
	Disconnected,
	Connected
};

UCLASS()
class GAME_API AOnlineGameMode : public AInGameMode
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AOnlineGameMode();
	virtual ~AOnlineGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** AOnlineGameMode : Start ***/
protected:
	EOnlineGameState OnlineGameState;

public:
	static const float CellSize;

	static int MaximumOfPioneers;

	float TimerOfSpaceShip;

protected:
	virtual void TickOfSpaceShip(float DeltaTime) override;

	void SpawnProjectile(class cInfoOfProjectile& InfoOfProjectile);

	

	/////////////////////////////////////////////////
	// Tick (Server)
	/////////////////////////////////////////////////
	UFUNCTION(Category = "AOnlineGameMode")
		void TickOfServerSocketInGame(float DeltaTime);

	void GetScoreBoard(float DeltaTime); float TimerOfGetScoreBoard;

	void SendInfoOfSpaceShip(float DeltaTime); float TimerOfSendInfoOfSpaceShip;

	void GetDiedPioneer(float DeltaTime); float TimerOfGetDiedPioneer;

	void GetInfoOfPioneer_Animation(float DeltaTime); float TimerOfGetInfoOfPioneer_Animation;
	void SetInfoOfPioneer_Animation(float DeltaTime); float TimerOfSetInfoOfPioneer_Animation;

	void GetInfoOfPioneer_Socket(float DeltaTime); float TimerOfGetInfoOfPioneer_Socket;

	void GetInfoOfPioneer_Stat(float DeltaTime); float TimerOfGetInfoOfPioneer_Stat;
	void SetInfoOfPioneer_Stat(float DeltaTime); float TimerOfSetInfoOfPioneer_Stat;

	void GetInfoOfProjectile(float DeltaTime); float TimerOfGetInfoOfProjectile;

	void SendInfoOfResources(float DeltaTime); float TimerOfSendInfoOfResources;

	void GetInfoOfBuilding_Spawn(float DeltaTime); float TimerOfGetInfoOfBuilding_Spawn;

	void SetInfoOfBuilding_Stat(float DeltaTime); float TimerOfSetInfoOfBuilding_Stat;

	void SetInfoOfEnemy_Animation(float DeltaTime); float TimerOfSetInfoOfEnemy_Animation;

	void SetInfoOfEnemy_Stat(float DeltaTime); float TimerOfSetInfoOfEnemy_Stat;


	/////////////////////////////////////////////////
	// Tick (Client)
	/////////////////////////////////////////////////
	UFUNCTION(Category = "AOnlineGameMode")
		void TickOfClientSocketInGame(float DeltaTime);

	void SendScoreBoard(float DeltaTime); float TimerOfSendScoreBoard;
	void RecvScoreBoard(float DeltaTime); float TimerOfRecvScoreBoard;

	void RecvInfoOfSpaceShip(float DeltaTime); float TimerOfRecvInfoOfSpaceShip;

	void RecvSpawnPioneer(float DeltaTime); float TimerOfRecvSpawnPioneer;
	void RecvDiedPioneer(float DeltaTime); float TimerOfRecvDiedPioneer;

	void SendInfoOfPioneer_Animation(float DeltaTime); float TimerOfSendInfoOfPioneer_Animation;
	void RecvInfoOfPioneer_Animation(float DeltaTime); float TimerOfRecvInfoOfPioneer_Animation;

	void RecvPossessPioneer(float DeltaTime); float TimerOfRecvPossessPioneer;

	void RecvInfoOfPioneer_Socket(float DeltaTime); float TimerOfRecvInfoOfPioneer_Socket;

	void SendInfoOfPioneer_Stat(float DeltaTime); float TimerOfSendInfoOfPioneer_Stat;
	void RecvInfoOfPioneer_Stat(float DeltaTime); float TimerOfRecvInfoOfPioneer_Stat;

	void RecvInfoOfProjectile(float DeltaTime); float TimerOfRecvInfoOfProjectile;

	void RecvInfoOfResources(float DeltaTime); float TimerOfRecvInfoOfResources;

	void RecvInfoOfBuilding_Spawn(float DeltaTime); float TimerOfRecvInfoOfBuilding_Spawn;

	void RecvInfoOfBuilding_Spawned(float DeltaTime); float TimerOfRecvInfoOfBuilding_Spawned;

	void SendInfoOfBuilding_Stat(float DeltaTime); float TimerOfSendInfoOfBuilding_Stat;
	void RecvInfoOfBuilding_Stat(float DeltaTime); float TimerOfRecvInfoOfBuilding_Stat;

	void RecvDestroyBuilding(float DeltaTime); float TimerOfRecvDestroyBuilding;


	void RecvSpawnEnemy(float DeltaTime); float TimerOfRecvSpawnEnemy;

	void SendInfoOfEnemy_Animation(float DeltaTime); float TimerOfSendInfoOfEnemy_Animation;
	void RecvInfoOfEnemy_Animation(float DeltaTime); float TimerOfRecvInfoOfEnemy_Animation;

	void SendInfoOfEnemy_Stat(float DeltaTime); float TimerOfSendInfoOfEnemy_Stat;
	void RecvInfoOfEnemy_Stat(float DeltaTime); float TimerOfRecvInfoOfEnemy_Stat;


	void RecvDestroyEnemy(float DeltaTime); float TimerOfRecvDestroyEnemy;
	void RecvExp(float DeltaTime); float TimerOfRecvExp;

	/////////////////////////////////////////////////
	// 패배 조건 확인
	/////////////////////////////////////////////////
	void CheckDefeatCondition(float DeltaTime); float TimerOfCheckDefeatCondition;

protected:


public:


/*** AOnlineGameMode : End ***/
};
