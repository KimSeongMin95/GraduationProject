// Fill out your copyright notice in the Description page of Project Settings.


#include "ParasiteLStarkie.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AParasiteLStarkie::AParasiteLStarkie()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(80.0f, 50.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemy/ParasiteLStarkie/ParasiteLStarkie_Idle.ParasiteLStarkie_Idle'"), 
		"AnimBlueprint'/Game/Characters/Enemy/ParasiteLStarkie/BP_ParasiteLStarkieAnimation.BP_ParasiteLStarkieAnimation_C'", 
		FVector(0.8f, 0.8f, 0.8f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -76.0f));
}

void AParasiteLStarkie::BeginPlay()
{
	Super::BeginPlay();

}

void AParasiteLStarkie::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AParasiteLStarkie::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(60, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AParasiteLStarkie::InitStat()
{
	HealthPoint = 60.0f;
	MaxHealthPoint = 60.0f;
	bDying = false;

	MoveSpeed = 12.0f;
	AttackSpeed = 2.0f;

	AttackPower = 10.0f;

	AttackRange = 3.0f;
	DetectRange = 32.0f;
	SightRange = 32.0f;
}


void AParasiteLStarkie::RunFSM()
{
	Super::RunFSM();

}

void AParasiteLStarkie::RunBehaviorTree()
{
	Super::RunBehaviorTree();

}
/*** ABaseCharacter : End ***/