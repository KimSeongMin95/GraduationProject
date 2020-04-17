// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeMan.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ATreeMan::ATreeMan()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(110.0f, 220.0f);

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

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 220.0f));
	HelthPointBar->SetDrawSize(FVector2D(140, 30));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void ATreeMan::InitStat()
{
	HealthPoint = 1500.0f;
	MaxHealthPoint = 1500.0f;
	bDying = false;

	MoveSpeed = 12.0f;
	AttackSpeed = 0.5f;

	AttackPower = 40.0f;

	AttackRange = 5.5f;
	DetectRange = 48.0f;
	SightRange = 48.0f;

	Exp = 21.0f;
}
/*** ABaseCharacter : End ***/