// Fill out your copyright notice in the Description page of Project Settings.


#include "ParasiteLStarkie.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AParasiteLStarkie::AParasiteLStarkie() // Sets default values
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(80.0f, 50.0f);

	GetCharacterMovement()->MaxWalkSpeed = 300.0f; // ������ �� �ȴ� �ӵ�

	InitStat();

	InitSkeletalAnimation();

	InitHelthPointBar();

	//InitFSM();

	DetactRangeSphereComp->SetSphereRadius(2048.0f);
	AttackDistance = 128.0f;
}

// Called when the game starts or when spawned
void AParasiteLStarkie::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AParasiteLStarkie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void AParasiteLStarkie::InitStat()
{
	HealthPoint = 30.0f;
	MaxHealthPoint = 30.0f;
	bDead = false;

	AttackPower = 8.0f;
	MoveSpeed = 3.0f;
	AttackSpeed = 2.0f;
	AttackRange = 4.0f;
	DetectRange = 8.0f;
	SightRange = 10.0f;
}
/*** Stat : End ***/

/*** HelthPointBar : Start ***/
void AParasiteLStarkie::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(60, 20));
}
/*** HelthPointBar : End ***/

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