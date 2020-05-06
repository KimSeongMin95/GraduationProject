// Fill out your copyright notice in the Description page of Project Settings.


#include "ParasiteZombie.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AParasiteZombie::AParasiteZombie()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(90.0f, 120.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/ParasiteZombie/Meshes/ParasiteZombie.ParasiteZombie'"), 
		"AnimBlueprint'/Game/Characters/Enemies/ParasiteZombie/Animations/ParasiteZombie_AnimBP.ParasiteZombie_AnimBP_C'", 
		FVector(1.1f, 1.1f, 1.1f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -118.0f));

	EnemyType = EEnemyType::ParasiteZombie;
}

void AParasiteZombie::BeginPlay()
{
	Super::BeginPlay();

}

void AParasiteZombie::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AParasiteZombie::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 105.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AParasiteZombie::InitStat()
{
	HealthPoint = 250.0f;
	MaxHealthPoint = 250.0f;
	bDying = false;

	MoveSpeed = 8.0f;
	AttackSpeed = 2.0f;

	AttackPower = 12.0f;

	AttackRange = 3.5f;
	DetectRange = 64.0f;
	SightRange = 64.0f;

	Exp = 9.0f;
}
/*** ABaseCharacter : End ***/