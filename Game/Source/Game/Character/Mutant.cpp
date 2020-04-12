// Fill out your copyright notice in the Description page of Project Settings.


#include "Mutant.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AMutant::AMutant()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(130.0f, 130.0f);

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

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 190.0f));
	HelthPointBar->SetDrawSize(FVector2D(120, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AMutant::InitStat()
{
	HealthPoint = 300.0f;
	MaxHealthPoint = 300.0f;
	bDying = false;

	MoveSpeed = 8.0f;
	AttackSpeed = 0.5f;

	AttackPower = 50.0f;

	AttackRange = 4.0f;
	DetectRange = 32.0f;
	SightRange = 32.0f;

	Exp = 9.0f;
}
/*** ABaseCharacter : End ***/