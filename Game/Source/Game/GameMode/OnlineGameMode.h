// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FClassFinder<T>
#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);
#include "Engine/Public/TimerManager.h" // For GetWorldTimerManager()

#include "CoreMinimal.h"
#include "GameMode/InGameMode.h"
#include "OnlineGameMode.generated.h"

UCLASS()
class GAME_API AOnlineGameMode : public AInGameMode
{
	GENERATED_BODY()

public:
	AOnlineGameMode();
	virtual ~AOnlineGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */
	virtual void Tick(float DeltaTime) override;

protected:
	/////////////////////////
	// 네트워크 통신 (Server)
	/////////////////////////
	UFUNCTION(Category = "AOnlineGameMode")
		void TickOfGameServer(float DeltaTime);

	void GetScoreBoard(float DeltaTime); 
	void SendInfoOfSpaceShip(float DeltaTime); 
	void GetDiedPioneer(float DeltaTime); 
	void GetInfoOfPioneer_Animation(float DeltaTime); 
	void SetInfoOfPioneer_Animation(float DeltaTime);
	void GetInfoOfPioneer_Socket(float DeltaTime);
	void GetInfoOfPioneer_Stat(float DeltaTime);
	void SetInfoOfPioneer_Stat(float DeltaTime);
	void GetInfoOfProjectile(float DeltaTime);
	void SendInfoOfResources(float DeltaTime);
	void GetInfoOfBuilding_Spawn(float DeltaTime);
	void SetInfoOfBuilding_Stat(float DeltaTime);
	void SetInfoOfEnemy_Animation(float DeltaTime);
	void SetInfoOfEnemy_Stat(float DeltaTime);

	/////////////////////////
	// 네트워크 통신 (Client)
	/////////////////////////
	UFUNCTION(Category = "AOnlineGameMode")
		void TickOfGameClient(float DeltaTime);

	void SendScoreBoard(float DeltaTime);
	void RecvScoreBoard(float DeltaTime);
	void RecvInfoOfSpaceShip(float DeltaTime);
	void RecvSpawnPioneer(float DeltaTime);
	void RecvDiedPioneer(float DeltaTime);
	void SendInfoOfPioneer_Animation(float DeltaTime);
	void RecvInfoOfPioneer_Animation(float DeltaTime);
	void RecvPossessPioneer(float DeltaTime);
	void RecvInfoOfPioneer_Socket(float DeltaTime);
	void SendInfoOfPioneer_Stat(float DeltaTime);
	void RecvInfoOfPioneer_Stat(float DeltaTime);
	void RecvInfoOfProjectile(float DeltaTime);
	void RecvInfoOfResources(float DeltaTime);
	void RecvInfoOfBuilding_Spawn(float DeltaTime);
	void RecvInfoOfBuilding_Spawned(float DeltaTime);
	void SendInfoOfBuilding_Stat(float DeltaTime);
	void RecvInfoOfBuilding_Stat(float DeltaTime);
	void RecvDestroyBuilding(float DeltaTime);
	void RecvSpawnEnemy(float DeltaTime);
	void SendInfoOfEnemy_Animation(float DeltaTime);
	void RecvInfoOfEnemy_Animation(float DeltaTime);
	void SendInfoOfEnemy_Stat(float DeltaTime);
	void RecvInfoOfEnemy_Stat(float DeltaTime);
	void RecvDestroyEnemy(float DeltaTime);
	void RecvExp(float DeltaTime);

	//////////////////////
	// 승리/패배 조건 확인
	//////////////////////
	virtual void CheckVictoryCondition(float DeltaTime);
	void CheckDefeatCondition(float DeltaTime);

	// 투사체 생성
	void SpawnProjectile(class CInfoOfProjectile& InfoOfProjectile);
};
