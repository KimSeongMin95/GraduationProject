// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonzombieTAvelange.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ASkeletonzombieTAvelange::ASkeletonzombieTAvelange() // Sets default values
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(80.0f, 50.0f);

	GetCharacterMovement()->MaxWalkSpeed = 100.0f; // ������ �� �ȴ� �ӵ�

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
	// USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Enemy/SkeletonzombieTAvelange/SkeletonzombieTAvelange_Idle.SkeletonzombieTAvelange_Idle'"));
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