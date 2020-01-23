// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonzombieTAvelange.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ASkeletonzombieTAvelange::ASkeletonzombieTAvelange()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(80.0f, 50.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemy/SkeletonzombieTAvelange/SkeletonzombieTAvelange_Idle.SkeletonzombieTAvelange_Idle'"), 
		"AnimBlueprint'/Game/Characters/Enemy/SkeletonzombieTAvelange/BP_SkeletonzombieTAvelangeAnimation.BP_SkeletonzombieTAvelangeAnimation_C'", 
		FVector(0.8f, 0.8f, 0.8f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -76.0f));
}

void ASkeletonzombieTAvelange::BeginPlay()
{
	Super::BeginPlay();

}

void ASkeletonzombieTAvelange::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ASkeletonzombieTAvelange::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void ASkeletonzombieTAvelange::InitStat()
{
	HealthPoint = 200.0f;
	MaxHealthPoint = 200.0f;
	bDying = false;

	MoveSpeed = 60.0f;
	AttackSpeed = 0.5f;

	AttackPower = 20.0f;

	AttackRange = 3.0f;
	DetectRange = 32.0f;
	SightRange = 32.0f;
}


void ASkeletonzombieTAvelange::RunFSM()
{
	Super::RunFSM();

}

void ASkeletonzombieTAvelange::RunBehaviorTree()
{
	Super::RunBehaviorTree();

}
/*** ABaseCharacter : End ***/