// Fill out your copyright notice in the Description page of Project Settings.


#include "GiantZombie.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AGiantZombie::AGiantZombie()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(190.0f, 190.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/GiantZombie/Meshes/GiantZombie.GiantZombie'"), 
		"AnimBlueprint'/Game/Characters/Enemies/GiantZombie/Animations/GiantZombie_AnimBP.GiantZombie_AnimBP_C'", 
		FVector(2.0f, 2.0f, 2.0f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -192.0f));

	EnemyType = EEnemyType::GiantZombie;
}

void AGiantZombie::BeginPlay()
{
	Super::BeginPlay();

}

void AGiantZombie::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AGiantZombie::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HelthPointBar->SetDrawSize(FVector2D(160, 30));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AGiantZombie::InitStat()
{
	HealthPoint = 500.0f;
	MaxHealthPoint = 500.0f;
	bDying = false;

	MoveSpeed = 8.0f;
	AttackSpeed = 0.5f;

	AttackPower = 50.0f;

	AttackRange = 5.5f;
	DetectRange = 32.0f;
	SightRange = 32.0f;

	Exp = 9.0f;
}
/*** ABaseCharacter : End ***/