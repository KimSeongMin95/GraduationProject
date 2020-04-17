// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeMan.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ATreeMan::ATreeMan()
{
	// 충돌 캡슐의 크기를 설정합니다.
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
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
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