// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonzombieTAvelange.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ASkeletonzombieTAvelange::ASkeletonzombieTAvelange() // Sets default values
{
	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(80.0f, 50.0f);

	GetCharacterMovement()->MaxWalkSpeed = 100.0f; // 움직일 때 걷는 속도

	InitStat();

	InitSkeletalAnimation();

	InitHelthPointBar();

	//InitFSM();

	DetactRangeSphereComp->SetSphereRadius(2048.0f);
	AttackRangeSphereComp->SetSphereRadius(128.0f);
}

// Called when the game starts or when spawned
void ASkeletonzombieTAvelange::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASkeletonzombieTAvelange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void ASkeletonzombieTAvelange::InitStat()
{
	HealthPoint = 80.0f;
	MaxHealthPoint = 80.0f;
	bDead = false;

	AttackPower = 12.0f;
	MoveSpeed = 2.0f;
	AttackSpeed = 0.5f;
	AttackRange = 4.0f;
	DetectRange = 8.0f;
	SightRange = 10.0f;
}
/*** Stat : End ***/

/*** HelthPointBar : Start ***/
void ASkeletonzombieTAvelange::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** HelthPointBar : End ***/

/*** SkeletalAnimation : Start ***/
void ASkeletonzombieTAvelange::InitSkeletalAnimation()
{
	// USkeletalMeshComponent에 USkeletalMesh을 설정합니다.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Enemy/SkeletonzombieTAvelange/SkeletonzombieTAvelange_Idle.SkeletonzombieTAvelange_Idle'"));
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
	FString animBP_Reference = "AnimBlueprint'/Game/Characters/Enemy/SkeletonzombieTAvelange/BP_SkeletonzombieTAvelangeAnimation.BP_SkeletonzombieTAvelangeAnimation_C'";
	UClass* animBP = LoadObject<UClass>(NULL, *animBP_Reference);
	if (!animBP)
	{
		UE_LOG(LogTemp, Warning, TEXT("!animBP"));
	}
	else
		GetMesh()->SetAnimInstanceClass(animBP);
}
/*** SkeletalAnimation : End ***/