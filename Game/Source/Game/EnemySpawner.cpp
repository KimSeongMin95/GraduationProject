// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/ParasiteLStarkie.h"
#include "Character/Prop2.h"
#include "Character/PumpkinhulkLShaw.h"
#include "Character/SkeletonzombieTAvelange.h"

#include "SpaceShip/SpaceShip.h"
/*** ���� ������ ��� ���� ���� : End ***/

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
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

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
	//SpawnParams.Name = TEXT("Name"); // Name�� �����մϴ�. World Outliner�� ǥ��Ǵ� Label���� �ٸ��ϴ�.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	/* SpawnParams.Instigator = Instigator;
	�𸮾� ������ ���� �����ӿ��� ��� ���Ϳ��� ������(Instigator)��� ������ �����Ǿ� �ֽ��ϴ�.
	�̴� ������ �������� ���� ������ ������ ���� �뵵�� ���Ǵµ�,
	�ݵ�� �������� ���� ���͸� ���������� �ʽ��ϴ�.
	���� �� �ڽ��� ������ ���Ͷ����, Ž���� �� �� �ֿ� ����� �����ϴµ� �����ϰ� ����� �� �ֽ��ϴ�.
	*/
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.
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