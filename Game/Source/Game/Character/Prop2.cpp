// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop2.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AProp2::AProp2() // Sets default values
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(140.0f, 80.0f);

	InitStat();

	AEnemy::InitRanges();

	AEnemy::InitCharacterMovement();

	InitSkeletalAnimation();

	InitHelthPointBar();

	//InitFSM();
}

// Called when the game starts or when spawned
void AProp2::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AProp2::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void AProp2::InitStat()
{
	HealthPoint = 250.0f;
	MaxHealthPoint = 250.0f;
	bDying = false;

	MoveSpeed = 8.0f;
	AttackSpeed = 1.0f;

	AttackPower = 40.0f;

	AttackRange = 4.0f;
	DetectRange = 8.0f;
	SightRange = 16.0f;
}
/*** Stat : End ***/

/*** IHealthPointBarInterface : Start ***/
void AProp2::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** IHealthPointBarInterface : End ***/

/*** SkeletalAnimation : Start ***/
void AProp2::InitSkeletalAnimation()
{
	// USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Enemy/Prop2/Mesh/Prop2.Prop2'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -142.0f));
	}

	// �� Enemy�� BP_Animation�� ��������. (������ ���� .BP_PioneerAnimation_C�� UAnimBluprint�� �ƴ� UClass�� �ҷ������ν� �ٷ� �����ϴ� ���Դϴ�.)
	FString animBP_Reference = "AnimBlueprint'/Game/Characters/Enemy/Prop2/Animations/BP_Prop2Animation.BP_Prop2Animation_C'";
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
void AProp2::RunFSM(float DeltaTime)
{
	Super::RunFSM(DeltaTime);

}
/*** FSM : End ***/

/*** BehaviorTree : Start ***/
void AProp2::RunBehaviorTree(float DeltaTime)
{
	Super::RunBehaviorTree(DeltaTime);

}
/*** BehaviorTree : End ***/