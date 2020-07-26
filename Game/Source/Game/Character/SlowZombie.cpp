// Fill out your copyright notice in the Description page of Project Settings.

#include "SlowZombie.h"

ASlowZombie::ASlowZombie()
{
	EnemyType = EEnemyType::SlowZombie;

	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(80.0f, 110.0f);

	InitHelthPointBar();

	InitStat();
	AEnemy::InitRanges();
	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/SlowZombie/Meshes/SlowZombie.SlowZombie'"), 
		"AnimBlueprint'/Game/Characters/Enemies/SlowZombie/Animations/SlowZombie_AnimBP.SlowZombie_AnimBP_C'", 
		FVector(1.1f, 1.1f, 1.1f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -109.0f));
}
ASlowZombie::~ASlowZombie()
{

}

void ASlowZombie::BeginPlay()
{
	Super::BeginPlay();
}
void ASlowZombie::Tick(float DeltaTime)
{
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}

void ASlowZombie::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}

void ASlowZombie::InitStat()
{
	HealthPoint = 400.0f;
	MaxHealthPoint = 400.0f;
	bDying = false;

	MoveSpeed = 3.0f;
	AttackSpeed = 0.5f;

	AttackPower = 15.0f;

	AttackRange = 3.5f;
	DetectRange = 64.0f;
	SightRange = 64.0f;

	Exp = 11.0f;
}
