// Fill out your copyright notice in the Description page of Project Settings.

#include "AlienAnimal.h"

#include "Network/NetworkComponent/Console.h"

AAlienAnimal::AAlienAnimal()
{
	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(90.0f, 100.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/AlienAnimal/Meshes/AlienAnimal.AlienAnimal'"),
		"AnimBlueprint'/Game/Characters/Enemies/AlienAnimal/Animations/AlienAnimal_AnimBP.AlienAnimal_AnimBP_C'",
		FVector(0.06f, 0.06f, 0.06f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -100.0f));

	EnemyType = EEnemyType::AlienAnimal;
}
AAlienAnimal::~AAlienAnimal()
{

}

void AAlienAnimal::BeginPlay()
{
	Super::BeginPlay();
}
void AAlienAnimal::Tick(float DeltaTime)
{
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}

void AAlienAnimal::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	HelthPointBar->SetDrawSize(FVector2D(90, 20));
}

void AAlienAnimal::InitStat()
{
	HealthPoint = 500.0f;
	MaxHealthPoint = 500.0f;
	bDying = false;

	MoveSpeed = 9.0f;
	AttackSpeed = 0.5f;

	AttackPower = 18.0f;

	AttackRange = 3.0f;
	DetectRange = 64.0f;
	SightRange = 64.0f;

	Exp = 13.0f;
}