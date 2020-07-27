// Fill out your copyright notice in the Description page of Project Settings.

#include "WarrokWKurniawan.h"

#include "Network/NetworkComponent/Console.h"
#include "GameMode/InGameMode.h"

AWarrokWKurniawan::AWarrokWKurniawan()
{
	EnemyType = EEnemyType::WarrokWKurniawan;

	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(200.0f, 220.0f);

	InitHelthPointBar();

	InitStat();
	AEnemy::InitRanges();
	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/WarrokWKurniawan/Meshes/WarrokWKurniawan.WarrokWKurniawan'"),
		"AnimBlueprint'/Game/Characters/Enemies/WarrokWKurniawan/Animations/WarrokWKurniawan_AnimBP.WarrokWKurniawan_AnimBP_C'",
		FVector(3.0f, 3.0f, 3.0f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -230.99f));
}
AWarrokWKurniawan::~AWarrokWKurniawan()
{

}

void AWarrokWKurniawan::BeginPlay()
{
	Super::BeginPlay();
}
void AWarrokWKurniawan::Tick(float DeltaTime)
{
	if (bDying)
		return;

	Super::Tick(DeltaTime);
}

void AWarrokWKurniawan::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 350.0f));
	HelthPointBar->SetDrawSize(FVector2D(150, 35));
}

void AWarrokWKurniawan::InitStat()
{
	HealthPoint = 10000.0f;
	MaxHealthPoint = 10000.0f;
	bDying = false;

	MoveSpeed = 11.0f;
	AttackSpeed = 0.5f;

	AttackPower = 100.0f;

	AttackRange = 7.5f;
	DetectRange = 64.0f;
	SightRange = 64.0f;

	Exp = 100.0f;
}

void AWarrokWKurniawan::Victory()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("<AWarrokWKurniawan::Victory()> if (!world)"));		
		return;
	}

	if (AInGameMode* inGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(world)))
	{
		inGameMode->ActivateInGameVictoryWidget();
	}
}
