// Fill out your copyright notice in the Description page of Project Settings.

#include "ParasiteZombie.h"

#include "Network/NetworkComponent/Console.h"

AParasiteZombie::AParasiteZombie()
{
	EnemyType = EEnemyType::ParasiteZombie;

	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(90.0f, 120.0f);

	InitHelthPointBar();

	InitStat();
	AEnemy::InitRanges();
	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/ParasiteZombie/Meshes/ParasiteZombie.ParasiteZombie'"), 
		"AnimBlueprint'/Game/Characters/Enemies/ParasiteZombie/Animations/ParasiteZombie_AnimBP.ParasiteZombie_AnimBP_C'", 
		FVector(1.1f, 1.1f, 1.1f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -118.0f));
}
AParasiteZombie::~AParasiteZombie()
{

}

void AParasiteZombie::BeginPlay()
{
	Super::BeginPlay();
}
void AParasiteZombie::Tick(float DeltaTime)
{
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}

void AParasiteZombie::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 105.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}

void AParasiteZombie::InitStat()
{
	HealthPoint = 250.0f;
	MaxHealthPoint = 250.0f;
	bDying = false;

	MoveSpeed = 8.0f;
	AttackSpeed = 2.0f;

	AttackPower = 12.0f;

	AttackRange = 3.5f;
	DetectRange = 64.0f;
	SightRange = 64.0f;

	Exp = 9.0f;
}
