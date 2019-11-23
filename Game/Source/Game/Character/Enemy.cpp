// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Controller/EnemyAIController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AEnemy::AEnemy() // Sets default values
{
	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(50.0f, 70.0f);

	InitSkeletalAnimation();
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAIController();

	// Init()이 끝나고 AIController에 빙의합니다.
	PossessAIController();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateTargetRotation(DeltaTime);

	if (bDead)
		Destroy();
}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void AEnemy::InitStat()
{
	State = EEnemyFSM::Idle;

	Health = 100.0f;
	bDead = false;

	AttackPower = 0.0f;
	MoveSpeed = 4.0f;
	AttackSpeed = 1.0f;
	AttackRange = 4.0f;
	DetectRange = 8.0f;
	SightRange = 10.0f;
}
/*** Stat : End ***/

/*** CharacterMovement : Start ***/
void AEnemy::RotateTargetRotation(float DeltaTime)
{
	// 회전을 할 필요가 없으면 실행하지 않습니다.
	if (!bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}
/*** CharacterMovement : End ***/

/*** SkeletalAnimation : Start ***/
void AEnemy::InitSkeletalAnimation()
{
	// USkeletalMeshComponent에 USkeletalMesh을 설정합니다.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Enemy/Prop2/Mesh/Prop2.Prop2'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
		GetMesh()->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -68.0f));
	}

	// 각 Enemy의 BP_Animation을 가져오기. (주의할 점은 .BP_PioneerAnimation_C로 UAnimBluprint가 아닌 UClass를 불러옴으로써 바로 적용하는 것입니다.)
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

/*** AEnemyAIController : Start ***/
void AEnemy::InitAIController()
{
	// 이미 AIController를 가지고 있으면 생성하지 않음.
	if (AIController)
		return;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = GetTransform(); // 현재 PioneerManager 객체 위치를 기반으로 합니다.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AIController = World->SpawnActor<AEnemyAIController>(AEnemyAIController::StaticClass(), myTrans, SpawnParams);
}

void AEnemy::PossessAIController()
{
	ABaseCharacter::PossessAIController();


}
/*** AEnemyAIController : End ***/