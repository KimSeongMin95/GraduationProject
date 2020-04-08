// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowZombie.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ASlowZombie::ASlowZombie()
{
	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(80.0f, 50.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/SlowZombie/Meshes/SlowZombie.SlowZombie'"), 
		"AnimBlueprint'/Game/Characters/Enemies/SlowZombie/Animations/SlowZombie_AnimBP.SlowZombie_AnimBP_C'", 
		FVector(0.8f, 0.8f, 0.8f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -76.0f));

	EnemyType = EEnemyType::SlowZombie;
}

void ASlowZombie::BeginPlay()
{
	Super::BeginPlay();

}

void ASlowZombie::Tick(float DeltaTime)
{
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
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

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void ASlowZombie::InitStat()
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

	Exp = 3.0f;
}
/*** ABaseCharacter : End ***/