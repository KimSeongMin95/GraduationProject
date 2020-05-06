// Fill out your copyright notice in the Description page of Project Settings.


#include "GiantZombie.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AGiantZombie::AGiantZombie()
{
	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(120.0f, 190.0f);

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

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HelthPointBar->SetDrawSize(FVector2D(160, 30));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AGiantZombie::InitStat()
{
	HealthPoint = 1000.0f;
	MaxHealthPoint = 1000.0f;
	bDying = false;

	MoveSpeed = 10.5f;
	AttackSpeed = 0.5f;

	AttackPower = 35.0f;

	AttackRange = 5.75f;
	DetectRange = 64.0f;
	SightRange = 64.0f;

	Exp = 19.0f;
}
/*** ABaseCharacter : End ***/