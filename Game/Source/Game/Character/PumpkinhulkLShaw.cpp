// Fill out your copyright notice in the Description page of Project Settings.


#include "PumpkinhulkLShaw.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
APumpkinhulkLShaw::APumpkinhulkLShaw() // Sets default values
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(140.0f, 80.0f);

	GetCharacterMovement()->MaxWalkSpeed = 300.0f; // ������ �� �ȴ� �ӵ�

	InitStat();

	InitSkeletalAnimation();

	InitHelthPointBar();

	//InitFSM();

	DetactRangeSphereComp->SetSphereRadius(2048.0f);
	AttackDistance = 256.0f;
}

// Called when the game starts or when spawned
void APumpkinhulkLShaw::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APumpkinhulkLShaw::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void APumpkinhulkLShaw::InitStat()
{
	HealthPoint = 200.0f;
	MaxHealthPoint = 200.0f;
	bDead = false;

	AttackPower = 30.0f;
	MoveSpeed = 2.0f;
	AttackSpeed = 0.5f;
	AttackRange = 4.0f;
	DetectRange = 8.0f;
	SightRange = 10.0f;
}
/*** Stat : End ***/

/*** HelthPointBar : Start ***/
void APumpkinhulkLShaw::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 190.0f));
	HelthPointBar->SetDrawSize(FVector2D(120, 20));
}
/*** HelthPointBar : End ***/

/*** SkeletalAnimation : Start ***/
void APumpkinhulkLShaw::InitSkeletalAnimation()
{
	// USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Enemy/PumpkinhulkLShaw/PumpkinhulkLShaw_Idle.PumpkinhulkLShaw_Idle'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -142.0f));
	}

	// �� Enemy�� BP_Animation�� ��������. (������ ���� .BP_PioneerAnimation_C�� UAnimBluprint�� �ƴ� UClass�� �ҷ������ν� �ٷ� �����ϴ� ���Դϴ�.)
	FString animBP_Reference = "AnimBlueprint'/Game/Characters/Enemy/PumpkinhulkLShaw/BP_PumpkinhulkLShawAnimation.BP_PumpkinhulkLShawAnimation_C'";
	UClass* animBP = LoadObject<UClass>(NULL, *animBP_Reference);
	if (!animBP)
	{
		UE_LOG(LogTemp, Warning, TEXT("!animBP"));
	}
	else
		GetMesh()->SetAnimInstanceClass(animBP);
}
/*** SkeletalAnimation : End ***/