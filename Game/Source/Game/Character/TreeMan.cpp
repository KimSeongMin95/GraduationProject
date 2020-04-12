// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeMan.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ATreeMan::ATreeMan()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(220.0f, 140.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/TreeMan/Meshes/treeman.TreeMan'"),
		"AnimBlueprint'/Game/Characters/Enemies/TreeMan/Animations/TreeMan_AnimBP.TreeMan_AnimBP_C'",
		FVector(1.0f, 1.0f, 1.0f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -222.0f));

	EnemyType = EEnemyType::TreeMan;
}

void ATreeMan::BeginPlay()
{
	Super::BeginPlay();

}

void ATreeMan::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ATreeMan::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 400.0f));
	HelthPointBar->SetDrawSize(FVector2D(200, 40));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void ATreeMan::InitStat()
{
	HealthPoint = 300.0f;
	MaxHealthPoint = 300.0f;
	bDying = false;

	MoveSpeed = 8.0f;
	AttackSpeed = 0.5f;

	AttackPower = 50.0f;

	AttackRange = 4.5f;
	DetectRange = 32.0f;
	SightRange = 32.0f;

	Exp = 9.0f;
}
/*** ABaseCharacter : End ***/