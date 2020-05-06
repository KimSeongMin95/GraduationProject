// Fill out your copyright notice in the Description page of Project Settings.


#include "ParasiteZombie.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AParasiteZombie::AParasiteZombie()
{
	// 충돌 캡슐의 크기를 설정합니다.
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
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
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