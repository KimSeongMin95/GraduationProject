// Fill out your copyright notice in the Description page of Project Settings.


#include "WarrokWKurniawan.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "GameMode/TutorialGameMode.h"
#include "GameMode/OnlineGameMode.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AWarrokWKurniawan::AWarrokWKurniawan()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(200.0f, 330.0f);

	InitHelthPointBar();

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation(TEXT("SkeletalMesh'/Game/Characters/Enemies/WarrokWKurniawan/Meshes/WarrokWKurniawan.WarrokWKurniawan'"),
		"AnimBlueprint'/Game/Characters/Enemies/WarrokWKurniawan/Animations/WarrokWKurniawan_AnimBP.WarrokWKurniawan_AnimBP_C'",
		FVector(3.0f, 3.0f, 3.0f), FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f, 0.0f, -330.99f));

	EnemyType = EEnemyType::WarrokWKurniawan;
}

void AWarrokWKurniawan::BeginPlay()
{
	Super::BeginPlay();

}

void AWarrokWKurniawan::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
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
	HealthPoint = 10000.0f;
	MaxHealthPoint = 10000.0f;
	bDying = false;

	MoveSpeed = 11.0f;
	AttackSpeed = 0.5f;

	AttackPower = 100.0f;

	AttackRange = 7.5f;
	DetectRange = 48.0f;
	SightRange = 48.0f;

	Exp = 100.0f;
}
/*** ABaseCharacter : End ***/


/*** AEnemy : Start ***/
void AWarrokWKurniawan::Victory()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AWarrokWKurniawan::Victory()> if (!world)"));
#endif				
		return;
	}

	ATutorialGameMode* tutorialGameMode = Cast<ATutorialGameMode>(UGameplayStatics::GetGameMode(world));
	AOnlineGameMode* onlineGameMode = Cast<AOnlineGameMode>(UGameplayStatics::GetGameMode(world));

	if (tutorialGameMode)
	{

	}
	else if (onlineGameMode)
	{
		onlineGameMode->ActivateInGameVictoryWidget();
	}
}
/*** AEnemy : End ***/