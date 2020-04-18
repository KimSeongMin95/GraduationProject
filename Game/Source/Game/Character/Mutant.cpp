// Fill out your copyright notice in the Description page of Project Settings.


#include "Mutant.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AMutant::AMutant()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(100.0f, 130.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/Mutant/Meshes/Mutant.Mutant'"),
		"AnimBlueprint'/Game/Characters/Enemies/Mutant/Animations/Mutant_AnimBP.Mutant_AnimBP_C'",
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -132.0f));

	EnemyType = EEnemyType::Mutant;
}

void AMutant::BeginPlay()
{
	Super::BeginPlay();

}

void AMutant::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AMutant::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 30));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AMutant::InitStat()
{
	HealthPoint = 800.0f;
	MaxHealthPoint = 800.0f;
	bDying = false;

	MoveSpeed = 10.0f;
	AttackSpeed = 0.5f;

	AttackPower = 25.0f;

	AttackRange = 4.5f;
	DetectRange = 48.0f;
	SightRange = 48.0f;

	Exp = 17.0f;
}
/*** ABaseCharacter : End ***/