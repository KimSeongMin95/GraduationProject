// Fill out your copyright notice in the Description page of Project Settings.


#include "WarrokWKurniawan.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AWarrokWKurniawan::AWarrokWKurniawan()
{
	// 충돌 캡슐의 크기를 설정합니다.
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
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
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