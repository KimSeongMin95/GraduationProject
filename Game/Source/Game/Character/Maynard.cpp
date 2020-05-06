// Fill out your copyright notice in the Description page of Project Settings.


#include "Maynard.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AMaynard::AMaynard()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(80.0f, 90.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/Maynard/Meshes/Maynard.Maynard'"),
		"AnimBlueprint'/Game/Characters/Enemies/Maynard/Animations/Maynard_AnimBP.Maynard_AnimBP_C'",
		FVector(1.1f, 1.1f, 1.1f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -90.0f));

	EnemyType = EEnemyType::Maynard;
}

void AMaynard::BeginPlay()
{
	Super::BeginPlay();

}

void AMaynard::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AMaynard::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AMaynard::InitStat()
{
	HealthPoint = 160.0f;
	MaxHealthPoint = 160.0f;
	bDying = false;

	MoveSpeed = 20.0f;
	AttackSpeed = 0.5f;

	AttackPower = 8.0f;

	AttackRange = 3.0f;
	DetectRange = 64.0f;
	SightRange = 64.0f;

	Exp = 7.0f;
}
/*** ABaseCharacter : End ***/