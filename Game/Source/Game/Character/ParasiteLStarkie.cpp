// Fill out your copyright notice in the Description page of Project Settings.


#include "ParasiteLStarkie.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AParasiteLStarkie::AParasiteLStarkie() // Sets default values
{
	// 충돌 캡슐의 크기를 설정합니다.
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
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
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
	// USkeletalMeshComponent에 USkeletalMesh을 설정합니다.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Enemy/ParasiteLStarkie/ParasiteLStarkie_Idle.ParasiteLStarkie_Idle'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
		GetMesh()->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.8f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -76.0f));
	}

	// 각 Enemy의 BP_Animation을 가져오기. (주의할 점은 .BP_PioneerAnimation_C로 UAnimBluprint가 아닌 UClass를 불러옴으로써 바로 적용하는 것입니다.)
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