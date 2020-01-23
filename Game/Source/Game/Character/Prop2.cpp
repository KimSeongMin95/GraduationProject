// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop2.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AProp2::AProp2()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(140.0f, 80.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemy/Prop2/Mesh/Prop2.Prop2'"), 
		"AnimBlueprint'/Game/Characters/Enemy/Prop2/Animations/BP_Prop2Animation.BP_Prop2Animation_C'", 
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, -142.0f));
}

void AProp2::BeginPlay()
{
	Super::BeginPlay();

}

void AProp2::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AProp2::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AProp2::InitStat()
{
	HealthPoint = 250.0f;
	MaxHealthPoint = 250.0f;
	bDying = false;

	MoveSpeed = 8.0f;
	AttackSpeed = 1.0f;

	AttackPower = 40.0f;

	AttackRange = 4.0f;
	DetectRange = 32.0f;
	SightRange = 32.0f;
}


void AProp2::RunFSM()
{
	Super::RunFSM();

}

void AProp2::RunBehaviorTree()
{
	Super::RunBehaviorTree();

}
/*** ABaseCharacter : End ***/