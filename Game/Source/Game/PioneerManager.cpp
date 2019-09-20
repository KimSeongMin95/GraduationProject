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

	// Instantiate Actor, GetWorld()->SpawnActor(�μ�)���� �μ��� ���� ���� �ʿ�
	APioneer* pioneer1 = (APioneer*)GetWorld()->SpawnActor(APioneer::StaticClass());
}

// Called every frame
void APioneerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// pioneer�� ID�� �ش��ϴ� ī�޶�� �����մϴ�.
void APioneerManager::ChangePioneerCameraByID(int pioneerID)
{
	// �ʿ� Ư�� Ű�� �ִ��� Ȯ���մϴ�. (���: �ʿ� Ű�� ����ִ��� Ȯ���� �� operator[]�� ����ؾ� �մϴ�.)
	if (!ListOfPioneerCamera.Contains(pioneerID))
		return;

	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	ListOfPioneerCamera[pioneerID];

	if (OurPlayerController && (ListOfPioneerCamera[pioneerID] != nullptr))
	{
		// ī�޶� ��� �����մϴ�.
		//OurPlayerController->SetViewTarget(ListOfPioneerCamera[pioneerID]);

		// ī�޶� �ε巴�� �����մϴ�.
		OurPlayerController->SetViewTargetWithBlend(ListOfPioneerCamera[pioneerID], 1.0f);
	}
}
