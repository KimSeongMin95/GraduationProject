// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"

// Sets default values
APioneerManager::APioneerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComp;

}

// Called when the game starts or when spawned
void APioneerManager::BeginPlay()
{
	Super::BeginPlay();

	SpawnPioneer(1);
	SpawnPioneer(2);

}

// Called every frame
void APioneerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void APioneerManager::SpawnPioneer(int ID)
{
	if (TmapPioneers.Contains(ID))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed: Pioneers->Contains(ID)"))
			return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Log, TEXT("Failed: UWorld* World = GetWorld();"))
			return;
	}

	FTransform myTrans = GetTransform();

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name");
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

	TmapPioneers.Emplace(ID, World->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams)); // 액터를 객체화 합니다.
}

APioneer* APioneerManager::GetPioneerByID(int ID)
{
	if (!TmapPioneers.Contains(ID))
		return nullptr;
	return TmapPioneers[ID];
}