// Fill out your copyright notice in the Description page of Project Settings.


#include "AlienAnimal.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AAlienAnimal::AAlienAnimal()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(100.0f, 100.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/AlienAnimal/Meshes/AlienAnimal.AlienAnimal'"),
		"AnimBlueprint'/Game/Characters/Enemies/AlienAnimal/Animations/AlienAnimal_AnimBP.AlienAnimal_AnimBP_C'",
		FVector(0.06f, 0.06f, 0.06f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -100.0f));

	EnemyType = EEnemyType::AlienAnimal;
}

void AAlienAnimal::BeginPlay()
{
	Super::BeginPlay();

}

void AAlienAnimal::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AAlienAnimal::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	HelthPointBar->SetDrawSize(FVector2D(90, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AAlienAnimal::InitStat()
{
	HealthPoint = 2000.0f;
	MaxHealthPoint = 2000.0f;
	bDying = false;

	MoveSpeed = 9.5f;
	AttackSpeed = 0.5f;

	AttackPower = 30.0f;

	AttackRange = 2.5f;
	DetectRange = 48.0f;
	SightRange = 48.0f;

	Exp = 9.0f;
}
/*** ABaseCharacter : End ***/