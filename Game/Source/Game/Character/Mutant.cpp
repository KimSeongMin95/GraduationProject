// Fill out your copyright notice in the Description page of Project Settings.

#include "Mutant.h"

AMutant::AMutant()
{
	EnemyType = EEnemyType::Mutant;

	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(100.0f, 130.0f);

	InitHelthPointBar();

	InitStat();
	AEnemy::InitRanges();
	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/Mutant/Meshes/Mutant.Mutant'"),
		"AnimBlueprint'/Game/Characters/Enemies/Mutant/Animations/Mutant_AnimBP.Mutant_AnimBP_C'",
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -132.0f));
}
AMutant::~AMutant()
{

}

void AMutant::BeginPlay()
{
	Super::BeginPlay();

}
void AMutant::Tick(float DeltaTime)
{
	if (bDying)
		return;

	Super::Tick(DeltaTime);
}

void AMutant::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 30));
}

void AMutant::InitStat()
{
	HealthPoint = 800.0f;
	MaxHealthPoint = 800.0f;
	bDying = false;

	MoveSpeed = 10.0f;
	AttackSpeed = 0.5f;

	AttackPower = 25.0f;

	AttackRange = 4.5f;
	DetectRange = 64.0f;
	SightRange = 64.0f;

	Exp = 17.0f;
}
