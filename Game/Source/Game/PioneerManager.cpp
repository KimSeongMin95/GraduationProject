// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"
#include "PioneerManager.h"
#include "Kismet/GameplayStatics.h" // 

// Sets default values
APioneerManager::APioneerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APioneerManager::BeginPlay()
{
	Super::BeginPlay();

	// Instantiate Actor, GetWorld()->SpawnActor(인수)에서 인수에 대한 정리 필요
	APioneer* pioneer1 = (APioneer*)GetWorld()->SpawnActor(APioneer::StaticClass());
}

// Called every frame
void APioneerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// pioneer의 ID에 해당하는 카메라로 변경합니다.
void APioneerManager::ChangePioneerCameraByID(int pioneerID)
{
	// 맵에 특정 키가 있는지 확인합니다. (경고: 맵에 키가 들어있는지 확인한 후 operator[]를 사용해야 합니다.)
	if (!ListOfPioneerCamera.Contains(pioneerID))
		return;

	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	ListOfPioneerCamera[pioneerID];

	if (OurPlayerController && (ListOfPioneerCamera[pioneerID] != nullptr))
	{
		// 카메라를 즉시 변경합니다.
		//OurPlayerController->SetViewTarget(ListOfPioneerCamera[pioneerID]);

		// 카메라를 부드럽게 변경합니다.
		OurPlayerController->SetViewTargetWithBlend(ListOfPioneerCamera[pioneerID], 1.0f);
	}
}
