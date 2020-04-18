// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotRaptor.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ARobotRaptor::ARobotRaptor()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(90.0f, 140.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/RobotRaptor/Meshes/RobotRaptor.RobotRaptor'"), 
		"AnimBlueprint'/Game/Characters/Enemies/RobotRaptor/Animations/RobotRaptor_AnimBP.RobotRaptor_AnimBP_C'", 
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, -142.0f));

	EnemyType = EEnemyType::RobotRaptor;
}

void ARobotRaptor::BeginPlay()
{
	Super::BeginPlay();

}

void ARobotRaptor::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ARobotRaptor::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 25));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void ARobotRaptor::InitStat()
{
	HealthPoint = 600.0f;
	MaxHealthPoint = 600.0f;
	bDying = false;

	MoveSpeed = 9.5f;
	AttackSpeed = 1.0f;

	AttackPower = 20.0f;

	AttackRange = 4.25f;
	DetectRange = 48.0f;
	SightRange = 48.0f;

	Exp = 15.0f;
}
/*** ABaseCharacter : End ***/