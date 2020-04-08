// Fill out your copyright notice in the Description page of Project Settings.


#include "GiantZombie.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AGiantZombie::AGiantZombie()
{
	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(140.0f, 80.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/GiantZombie/Meshes/GiantZombie.GiantZombie'"), 
		"AnimBlueprint'/Game/Characters/Enemies/GiantZombie/Animations/GiantZombie_AnimBP.GiantZombie_AnimBP_C'", 
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -142.0f));

	EnemyType = EEnemyType::GiantZombie;
}

void AGiantZombie::BeginPlay()
{
	Super::BeginPlay();

}

void AGiantZombie::Tick(float DeltaTime)
{
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
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

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 190.0f));
	HelthPointBar->SetDrawSize(FVector2D(120, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AGiantZombie::InitStat()
{
	HealthPoint = 300.0f;
	MaxHealthPoint = 300.0f;
	bDying = false;

	MoveSpeed = 8.0f;
	AttackSpeed = 0.5f;

	AttackPower = 50.0f;

	AttackRange = 4.0f;
	DetectRange = 32.0f;
	SightRange = 32.0f;

	Exp = 9.0f;
}
/*** ABaseCharacter : End ***/