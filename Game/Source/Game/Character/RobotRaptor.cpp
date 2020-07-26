// Fill out your copyright notice in the Description page of Project Settings.

#include "RobotRaptor.h"

ARobotRaptor::ARobotRaptor()
{
	EnemyType = EEnemyType::RobotRaptor;

	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(90.0f, 140.0f);

	InitHelthPointBar();

	InitStat();
	AEnemy::InitRanges();
	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/RobotRaptor/Meshes/RobotRaptor.RobotRaptor'"), 
		"AnimBlueprint'/Game/Characters/Enemies/RobotRaptor/Animations/RobotRaptor_AnimBP.RobotRaptor_AnimBP_C'", 
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, -142.0f));
}
ARobotRaptor::~ARobotRaptor()
{

}

void ARobotRaptor::BeginPlay()
{
	Super::BeginPlay();
}
void ARobotRaptor::Tick(float DeltaTime)
{
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}

void ARobotRaptor::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 25));
}

void ARobotRaptor::InitStat()
{
	HealthPoint = 600.0f;
	MaxHealthPoint = 600.0f;
	bDying = false;

	MoveSpeed = 9.5f;
	AttackSpeed = 1.0f;

	AttackPower = 20.0f;

	AttackRange = 4.25f;
	DetectRange = 64.0f;
	SightRange = 64.0f;

	Exp = 15.0f;
}
