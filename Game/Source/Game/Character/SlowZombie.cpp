// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowZombie.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ASlowZombie::ASlowZombie()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(110.0f, 90.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/SlowZombie/Meshes/SlowZombie.SlowZombie'"), 
		"AnimBlueprint'/Game/Characters/Enemies/SlowZombie/Animations/SlowZombie_AnimBP.SlowZombie_AnimBP_C'", 
		FVector(1.1f, 1.1f, 1.1f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -109.0f));

	EnemyType = EEnemyType::SlowZombie;
}

void ASlowZombie::BeginPlay()
{
	Super::BeginPlay();

}

void ASlowZombie::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ASlowZombie::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void ASlowZombie::InitStat()
{
	HealthPoint = 200.0f;
	MaxHealthPoint = 200.0f;
	bDying = false;

	MoveSpeed = 6.0f;
	AttackSpeed = 0.5f;

	AttackPower = 20.0f;

	AttackRange = 3.0f;
	DetectRange = 48.0f;
	SightRange = 48.0f;

	Exp = 3.0f;
}
/*** ABaseCharacter : End ***/