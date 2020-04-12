// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
#include "Character/SlowZombie.h"
#include "Character/ParasiteZombie.h"
#include "Character/GiantZombie.h"
#include "Character/RobotRaptor.h"
#include "Character/WarrokWKurniawan.h"
#include "Character/TreeMan.h"
#include "Character/Mutant.h"
#include "Character/Maynard.h"
#include "Character/AlienAnimal.h"

#include "Network/Packet.h"
#include "Network/ServerSocketInGame.h"

#include "EnemySpawner.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<AEnemyManager::BeginPlay()> if (!world)"));
#endif
		return;
	}


	ServerSocketInGame = cServerSocketInGame::GetSingleton();

	// 에디터에서 월드상에 배치한 Building들을 관리하기 위해 추가합니다.
	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			for (TActorIterator<AEnemy> ActorItr(world); ActorItr; ++ActorItr)
			{
				(*ActorItr)->ID = ID;

				Enemies.Add(ID, *ActorItr);

				ID++;

				(*ActorItr)->SetEnemyManager(this);

				ServerSocketInGame->SendSpawnEnemy((*ActorItr)->GetInfoOfEnemy());
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
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<AEnemyManager::SpawnEnemy(...)> if (!world)"));
#endif
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AEnemy* enemy = nullptr;

	switch ((EEnemyType)EnemyType)
	{
	case EEnemyType::SlowZombie:
		enemy = world->SpawnActor<ASlowZombie>(ASlowZombie::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::ParasiteZombie:
		enemy = world->SpawnActor<AParasiteZombie>(AParasiteZombie::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::GiantZombie:
		enemy = world->SpawnActor<AGiantZombie>(AGiantZombie::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::RobotRaptor:
		enemy = world->SpawnActor<ARobotRaptor>(ARobotRaptor::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::WarrokWKurniawan:
		enemy = world->SpawnActor<AWarrokWKurniawan>(AWarrokWKurniawan::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::TreeMan:
		enemy = world->SpawnActor<ATreeMan>(ATreeMan::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::Mutant:
		enemy = world->SpawnActor<AMutant>(AMutant::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::Maynard:
		enemy = world->SpawnActor<AMaynard>(AMaynard::StaticClass(), Transform, SpawnParams);
		break;
	case EEnemyType::AlienAnimal:
		enemy = world->SpawnActor<AAlienAnimal>(AAlienAnimal::StaticClass(), Transform, SpawnParams);
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

		if (ServerSocketInGame)
		{
			if (ServerSocketInGame->IsServerOn())
			{
				ServerSocketInGame->SendSpawnEnemy(enemy->GetInfoOfEnemy());
			}
		}
	}

	return enemy;
}

void AEnemyManager::RecvSpawnEnemy(class cInfoOfEnemy& InfoOfEnemy)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<AEnemyManager::RecvSpawnEnemy(...)> if (!world)"));
#endif
		return;
	}

	FTransform myTrans = InfoOfEnemy.Animation.GetActorTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.


	AEnemy* enemy = nullptr;

	switch ((EEnemyType)InfoOfEnemy.Spawn.EnemyType)
	{
	case EEnemyType::SlowZombie:
		enemy = world->SpawnActor<ASlowZombie>(ASlowZombie::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::ParasiteZombie:
		enemy = world->SpawnActor<AParasiteZombie>(AParasiteZombie::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::GiantZombie:
		enemy = world->SpawnActor<AGiantZombie>(AGiantZombie::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::RobotRaptor:
		enemy = world->SpawnActor<ARobotRaptor>(ARobotRaptor::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::WarrokWKurniawan:
		enemy = world->SpawnActor<AWarrokWKurniawan>(AWarrokWKurniawan::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::TreeMan:
		enemy = world->SpawnActor<ATreeMan>(ATreeMan::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::Mutant:
		enemy = world->SpawnActor<AMutant>(AMutant::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::Maynard:
		enemy = world->SpawnActor<AMaynard>(AMaynard::StaticClass(), myTrans, SpawnParams);
		break;
	case EEnemyType::AlienAnimal:
		enemy = world->SpawnActor<AAlienAnimal>(AAlienAnimal::StaticClass(), myTrans, SpawnParams);
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