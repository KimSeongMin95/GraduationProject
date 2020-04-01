// Fill out your copyright notice in the Description page of Project Settings.


#include "Gate.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AGate::AGate()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	BuildingType = EBuildingType::Gate;

	InitTriggerOfGate();
}

void AGate::BeginPlay()
{
	Super::BeginPlay();

}

void AGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BuildingState == EBuildingState::Constructed)
		TickOfOpenAndCloseTheGate(DeltaTime);
}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AGate::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void AGate::InitStat()
{
	ConstructionTime = 20.0f;

	HealthPoint = 200.0f;
	MaxHealthPoint = 1000.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 300.0f;
	NeedOrganicMatter = 50.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 2.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}

void AGate::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(16.907f, 1.0f, 8.3f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}

void AGate::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Gate/SM_Doorway_L.SM_Doorway_L'"),
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	
	AddBuildingSMC(&BuildingSMC_2, TEXT("BuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Gate/SM_Door_L.SM_Door_L'"),
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	AddBuildingSMC(&LeftSideWall, TEXT("LeftSideWall"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Hall_Win_Det_A.SM_Hall_Win_Det_A'"),
		FVector(1.5f, 2.092f, 1.745f), FRotator(0.0f, 0.0f, 0.0f), FVector(-665.136f, 0.0f, 0.0f));

	AddBuildingSMC(&RightSideWall, TEXT("RightSideWall"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Hall_Win_Det_A.SM_Hall_Win_Det_A'"),
		FVector(1.5f, 2.092f, 1.745f), FRotator(0.0f, 180.0f, 0.0f), FVector(642.864f, -105.086f, 0.0f));

	AddBuildingSMC(&Ceiling, TEXT("Ceiling"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularSci_Int/Meshes/SM_Ceiling_B_2.SM_Ceiling_B_2'"),
		FVector(4.085f, 0.272f, 1.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 611.258f));
}

void AGate::InitTriggerOfGate()
{
	TriggerOfGate = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerOfGate"));
	TriggerOfGate->SetupAttachment(RootComponent);

	TriggerOfGate->SetSphereRadius(650.0f);

	TriggerOfGate->OnComponentBeginOverlap.AddDynamic(this, &AGate::OnOverlapBegin);
	TriggerOfGate->OnComponentEndOverlap.AddDynamic(this, &AGate::OnOverlapEnd);
}

void AGate::OpenTheGate(float DeltaTime)
{
	FTransform transform = BuildingSMC_2->GetRelativeTransform();

	transform.AddToTranslation(BuildingSMC_2->GetUpVector() * DeltaTime * 128.0f);

	FVector location = transform.GetLocation();

	if (location.Z >= 450.0f)
		location.Z = 450.0f;

	transform.SetLocation(location);

	BuildingSMC_2->SetRelativeTransform(transform);
}

void AGate::CloseTheGate(float DeltaTime)
{
	FTransform transform = BuildingSMC_2->GetRelativeTransform();

	transform.AddToTranslation(BuildingSMC_2->GetUpVector() * DeltaTime * -128.0f);

	FVector location = transform.GetLocation();

	if (location.Z < 0.0f)
		location.Z = 0.0f;

	transform.SetLocation(location);

	BuildingSMC_2->SetRelativeTransform(transform);
}

void AGate::TickOfOpenAndCloseTheGate(float DeltaTime)
{
	if (!BuildingSMC_2)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AGate::TickOfOpenAndCloseTheGate(...)> if (!BuildingSMC_2)"));
#endif
		return;
	}

	if (OverlappedPioneers.Num() >= 1)
	{
		OpenTheGate(DeltaTime);
		return;
	}

	// Pioneer는 없지만 Enemy가 있을 때
	for (AActor* enemy : OverlappedEnemies)
	{
		float distance = FVector::Distance(GetActorLocation(), enemy->GetActorLocation());

		// Gate와의 거리가 300.0f 이하면 문에 깔릴 수 있으므로 닫지 않습니다.
		if (distance < 300.0f)
			return;
	}
	
	CloseTheGate(DeltaTime);
}

void AGate::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (OtherComp->IsA(UCapsuleComponent::StaticClass()))
		{
			OverlappedPioneers.Add(OtherActor);
		}
	}

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (OtherComp->IsA(UCapsuleComponent::StaticClass()))
		{
			OverlappedEnemies.Add(OtherActor);
		}
	}
}

void AGate::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (OtherComp->IsA(UCapsuleComponent::StaticClass()))
		{
			OverlappedPioneers.RemoveSingle(OtherActor);
		}
	}

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (OtherComp->IsA(UCapsuleComponent::StaticClass()))
		{
			OverlappedEnemies.RemoveSingle(OtherActor);
		}
	}
}
/*** ABuilding : End ***/