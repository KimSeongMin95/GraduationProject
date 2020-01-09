// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/ParasiteLStarkie.h"
#include "Character/Prop2.h"
#include "Character/PumpkinhulkLShaw.h"
#include "Character/SkeletonzombieTAvelange.h"

#include "SpaceShip/SpaceShip.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComp;

	Type = EEnemyType::Type1;
	SpawnNowLimit = 5;
	SpawnCount = 0;
	SpawnTotalLimit = 15;
	SpawnTimer = 0.0f;
	SpawnTime = 3.0f;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &AEnemySpawner::ClearDeadEnemy, 1.0f, true);
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpawnEnemy(DeltaTime);

}
/*** Basic Function : End ***/

void AEnemySpawner::SpawnEnemy(float DeltaTime)
{
	if (Enemies.Num() >= SpawnNowLimit || SpawnCount >= SpawnTotalLimit)
	{
		if (Type == EEnemyType::Type1 && bbb == false)
		{
			bbb = true;


			UWorld* const World = GetWorld();
			if (!World)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
				return;
			}

			FTransform myTrans;
			myTrans.SetLocation(FVector(-8018.74f, -6935.93f, 25000.0f));

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

			World->SpawnActor<ASpaceShip>(ASpaceShip::StaticClass(), myTrans, SpawnParams);
		}

		return;
	}

	SpawnTimer += DeltaTime;
	if (SpawnTimer < SpawnTime)
		return;

	SpawnTimer = 0.0f;
	SpawnCount++;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = GetActorTransform();

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	/* SpawnParams.Instigator = Instigator;
	언리얼 엔진의 게임 프레임웍의 모든 액터에는 가해자(Instigator)라는 변수가 설정되어 있습니다.
	이는 나에게 데미지를 가한 액터의 정보를 보관 용도로 사용되는데,
	반드시 데미지를 가한 액터만 보관하지는 않습니다.
	예를 들어서 자신을 스폰한 액터라던지, 탐지할 적 등 주요 대상을 저장하는데 유용하게 사용할 수 있습니다.
	*/
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.
	///** Fall back to default settings. */
	//Undefined								UMETA(DisplayName = "Default"),
	///** Actor will spawn in desired location, regardless of collisions. */
	//AlwaysSpawn								UMETA(DisplayName = "Always Spawn, Ignore Collisions"),
	///** Actor will try to find a nearby non-colliding location (based on shape components), but will always spawn even if one cannot be found. */
	//AdjustIfPossibleButAlwaysSpawn			UMETA(DisplayName = "Try To Adjust Location, But Always Spawn"),
	///** Actor will try to find a nearby non-colliding location (based on shape components), but will NOT spawn unless one is found. */
	//AdjustIfPossibleButDontSpawnIfColliding	UMETA(DisplayName = "Try To Adjust Location, Don't Spawn If Still Colliding"),
	///** Actor will fail to spawn. */
	//DontSpawnIfColliding					UMETA(DisplayName = "Do Not Spawn"),

	switch(Type)
	{
		case EEnemyType::Type1:
		{
			Enemies.Add(World->SpawnActor<AParasiteLStarkie>(AParasiteLStarkie::StaticClass(), myTrans, SpawnParams));
		}
		break;
		case EEnemyType::Type2:
		{
			Enemies.Add(World->SpawnActor<AProp2>(AProp2::StaticClass(), myTrans, SpawnParams));
		}
		break;
		case EEnemyType::Type3:
		{
			Enemies.Add(World->SpawnActor<APumpkinhulkLShaw>(APumpkinhulkLShaw::StaticClass(), myTrans, SpawnParams));
		}
		break;
		case EEnemyType::Type4:
		{
			Enemies.Add(World->SpawnActor<ASkeletonzombieTAvelange>(ASkeletonzombieTAvelange::StaticClass(), myTrans, SpawnParams));
		}
		break;
	}

	
}

void AEnemySpawner::ClearDeadEnemy()
{
	/*for (AActor* enemy : Enemies)
	{
		if (!enemy)
			continue;

		if (enemy->IsActorBeingDestroyed())
			Enemies.RemoveSingle(enemy);
	}*/
}