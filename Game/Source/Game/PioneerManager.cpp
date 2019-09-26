// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"
#include "Components/SceneComponent.h"
#include "Pioneer.h"
#include "Engine/World.h"

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

	SpawnPioneers(10);
}

// Called every frame
void APioneerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APioneerManager::SpawnPioneers(int num)
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	for (int i = 0; i < num; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
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
		FTransform myTrans = GetTransform();

		APioneer* pio = World->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

		if (pio)
		{
			// Do something..
		}
	}

}