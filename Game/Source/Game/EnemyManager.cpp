// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Character/SlowZombie.h"
#include "Character/ParasiteZombie.h"
#include "Character/GiantZombie.h"
#include "Character/RobotRaptor.h"

#include "Network/Packet.h"
#include "Network/ServerSocketInGame.h"

#include "EnemySpawner.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ID = 1;
}

void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	

	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AEnemyManager::BeginPlay()> if (!world)\n");
		return;
	}


	ServerSocketInGame = cServerSocketInGame::GetSingleton();

	// �����Ϳ��� ����� ��ġ�� Building���� �����ϱ� ���� �߰��մϴ�.
	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			for (TActorIterator<AEnemy> ActorItr(world); ActorItr; ++ActorItr)
			{
				(*ActorItr)->ID = ID;

				Enemies.Add(ID, *ActorItr);

				ID++;

				//ServerSocketInGame->SendSpawnEnemy((*ActorItr)->GetInfoOfEnemy());
			}
		}
	}


	for (TActorIterator<AEnemySpawner> ActorItr(world); ActorItr; ++ActorItr)
	{
		(*ActorItr)->SetEnemyManager(this);

		EnemySpawners.Add(*ActorItr);
	}
}

void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AEnemyManager : Start ***/
class AEnemy* AEnemyManager::SpawnEnemy(int EnemyType, FTransform Transform)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyManager::SpawnEnemy: if (!World)"));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AEnemy* enemy = nullptr;

	switch ((EEnemyType)EnemyType)
	{
	case EEnemyType::SlowZombie:
		enemy = World->SpawnActor<ASlowZombie>(ASlowZombie::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::ParasiteZombie:
		enemy = World->SpawnActor<AParasiteZombie>(AParasiteZombie::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::GiantZombie:
		enemy = World->SpawnActor<AGiantZombie>(AGiantZombie::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::RobotRaptor:
		enemy = World->SpawnActor<ARobotRaptor>(ARobotRaptor::StaticClass(), Transform, SpawnParams);
		break;

	default:
		
		break;
	}

	if (enemy)
	{
		enemy->ID = ID;

		if (Enemies.Contains(ID) == false)
			Enemies.Add(ID, enemy);

		ID++;

		enemy->SetEnemyManager(this);
	}

	return enemy;
}

void AEnemyManager::RecvSpawnEnemy(class cInfoOfEnemy& InfoOfEnemy)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyManager::RecvSpawnEnemy: if (!World)"));
		return;
	}

	FTransform myTrans = InfoOfEnemy.Animation.GetActorTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.


	AEnemy* enemy = nullptr;

	switch ((EEnemyType)InfoOfEnemy.Spawn.EnemyType)
	{
	case EEnemyType::SlowZombie:
		enemy = World->SpawnActor<ASlowZombie>(ASlowZombie::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::ParasiteZombie:
		enemy = World->SpawnActor<AParasiteZombie>(AParasiteZombie::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::GiantZombie:
		enemy = World->SpawnActor<AGiantZombie>(AGiantZombie::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::RobotRaptor:
		enemy = World->SpawnActor<ARobotRaptor>(ARobotRaptor::StaticClass(), myTrans, SpawnParams);
		break;

	default:

		break;
	}

	if (enemy)
	{
		enemy->ID = InfoOfEnemy.ID;

		if (Enemies.Contains(InfoOfEnemy.ID) == false)
			Enemies.Add(InfoOfEnemy.ID, enemy);


		enemy->SetEnemyManager(this);
	}
}
/*** AEnemyManager : End ***/