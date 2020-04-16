// Fill out your copyright notice in the Description page of Project Settings.


#include "WarrokWKurniawan.h"


/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AWarrokWKurniawan::AWarrokWKurniawan()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(220.0f, 210.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/WarrokWKurniawan/Meshes/WarrokWKurniawan.WarrokWKurniawan'"),
		"AnimBlueprint'/Game/Characters/Enemies/WarrokWKurniawan/Animations/WarrokWKurniawan_AnimBP.WarrokWKurniawan_AnimBP_C'",
		FVector(2.0f, 2.0f, 2.0f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -225.0f));

	EnemyType = EEnemyType::WarrokWKurniawan;
}

void AWarrokWKurniawan::BeginPlay()
{
	Super::BeginPlay();

}

void AWarrokWKurniawan::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AWarrokWKurniawan::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 220.0f));
	HelthPointBar->SetDrawSize(FVector2D(150, 35));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AWarrokWKurniawan::InitStat()
{
	HealthPoint = 2000.0f;
	MaxHealthPoint = 2000.0f;
	bDying = false;

	MoveSpeed = 8.0f;
	AttackSpeed = 0.5f;

	AttackPower = 100.0f;

	AttackRange = 5.8f;
	DetectRange = 48.0f;
	SightRange = 48.0f;

	Exp = 9.0f;
}
/*** ABaseCharacter : End ***/