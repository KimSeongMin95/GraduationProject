// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

/*** Basic Function : Start ***/
// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitItem();
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

void AItem::InitItem()
{
	State = EItemState::Droped;

	HalfHeightOfBox = 5.0f; // 너무 작으면 충돌이 제대로 이루어지지 않음.

	RadiusOfItem = 192.0f;

	PhysicsBoxComp = CreateDefaultSubobject<UBoxComponent>("PhysicsBoxComp");
	RootComponent = PhysicsBoxComp;
	PhysicsBoxComp->SetBoxExtent(FVector(RadiusOfItem / 2.0f, RadiusOfItem / 2.0f, HalfHeightOfBox), false);
	PhysicsBoxComp->SetGenerateOverlapEvents(false);
	PhysicsBoxComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PhysicsBoxComp->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	PhysicsBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PhysicsBoxComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	PhysicsBoxComp->SetSimulatePhysics(true);
	PhysicsBoxComp->SetRelativeLocation(FVector(0.0f, 0.0f, HalfHeightOfBox));

	InteractionRange = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRange"));
	InteractionRange->SetupAttachment(RootComponent);
	InteractionRange->SetGenerateOverlapEvents(true);
	InteractionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionRange->SetSphereRadius(RadiusOfItem);

	StaticMeshOfItem = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshOfItem"));
	StaticMeshOfItem->SetupAttachment(RootComponent);
	StaticMeshOfItem->SetGenerateOverlapEvents(false);
	StaticMeshOfItem->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItem::InitStaticMeshOfItem(const TCHAR* ObjectToFind, FRotator Rotation, FVector Location)
{
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(ObjectToFind);
	if (staticMesh.Succeeded())
	{
		StaticMeshOfItem->SetStaticMesh(staticMesh.Object);

		// StaticMesh의 원본 사이즈 측정
		FVector minBounds, maxBounds;
		StaticMeshOfItem->GetLocalBounds(minBounds, maxBounds);

		// StaticMeshOfItem의 사이즈를 통일하기 위해 메시의 최대 사이즈로 구한 scale을 일괄적으로 적용
		float gap[3] = { maxBounds.X - minBounds.X, maxBounds.Y - minBounds.Y, maxBounds.Z - minBounds.Z };
		float maxOfGap = 0.0f;
		for (float g : gap)
		{
			if (g > maxOfGap)
				maxOfGap = g;
		}
		float scaleOfItem = (maxOfGap != 0.0f) ? (RadiusOfItem / maxOfGap) : 1.0f;
		FVector Scale(scaleOfItem, scaleOfItem, scaleOfItem);

		//FRotator Rotation(-45.0f, 0.0f, 0.0f); // 플레이어에게 잘 보이도록 45도 기울임
		//FVector Location(0.0f, 0.0f, 0.0f);

		// RootComponent인 SphereComponent가 StaticMesh의 하단 정중앙으로 오게끔 설정해줘야 함.
		// 순서는 S->R->T 순으로 해야 원점에서 벗어나지 않음.
		StaticMeshOfItem->SetRelativeScale3D(Scale);
		StaticMeshOfItem->SetRelativeRotation(Rotation);
		FVector center;
		center.X = -1.0f * (((maxBounds.X + minBounds.X) * Scale.X) / 2.0f);
		center.Y = -1.0f * (((maxBounds.Y + minBounds.Y) * Scale.Y) / 2.0f);
		center.Z = -1.0f * (minBounds.Z * Scale.Z);
		StaticMeshOfItem->SetRelativeLocation(center + Location);
	}
}

void AItem::Droped()
{
	State = EItemState::Droped;

	if (PhysicsBoxComp)
		PhysicsBoxComp->SetSimulatePhysics(true);

	if (InteractionRange)
		InteractionRange->SetGenerateOverlapEvents(true);

	if (StaticMeshOfItem)
		StaticMeshOfItem->SetHiddenInGame(false);
}
void AItem::Acquired()
{
	State = EItemState::Acquired;

	if (PhysicsBoxComp)
		PhysicsBoxComp->SetSimulatePhysics(false);

	if (InteractionRange)
		InteractionRange->SetGenerateOverlapEvents(false);

	if (StaticMeshOfItem)
		StaticMeshOfItem->SetHiddenInGame(true);
}