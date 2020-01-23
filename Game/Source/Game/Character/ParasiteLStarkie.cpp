// Fill out your copyright notice in the Description page of Project Settings.


#include "ParasiteLStarkie.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AParasiteLStarkie::AParasiteLStarkie() // Sets default values
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(80.0f, 50.0f);

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation();

	InitHelthPointBar();

	//InitFSM();
}

// Called when the game starts or when spawned
void AParasiteLStarkie::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AParasiteLStarkie::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Stat : Start ***/
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
/*** Stat : End ***/

/*** IHealthPointBarInterface : Start ***/
void AParasiteLStarkie::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(60, 20));
}
/*** IHealthPointBarInterface : End ***/

/*** SkeletalAnimation : Start ***/
void AParasiteLStarkie::InitSkeletalAnimation()
{
	// USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Enemy/ParasiteLStarkie/ParasiteLStarkie_Idle.ParasiteLStarkie_Idle'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.8f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -76.0f));
	}

	// �� Enemy�� BP_Animation�� ��������. (������ ���� .BP_PioneerAnimation_C�� UAnimBluprint�� �ƴ� UClass�� �ҷ������ν� �ٷ� �����ϴ� ���Դϴ�.)
	FString animBP_Reference = "AnimBlueprint'/Game/Characters/Enemy/ParasiteLStarkie/BP_ParasiteLStarkieAnimation.BP_ParasiteLStarkieAnimation_C'";
	UClass* animBP = LoadObject<UClass>(NULL, *animBP_Reference);
	if (!animBP)
	{
		UE_LOG(LogTemp, Warning, TEXT("!animBP"));
	}
	else
		GetMesh()->SetAnimInstanceClass(animBP);
}
/*** SkeletalAnimation : End ***/

/*** FSM : Start ***/
void AParasiteLStarkie::RunFSM()
{
	Super::RunFSM();

}
/*** FSM : End ***/

/*** BehaviorTree : Start ***/
void AParasiteLStarkie::RunBehaviorTree()
{
	Super::RunBehaviorTree();

}
/*** BehaviorTree : End ***/