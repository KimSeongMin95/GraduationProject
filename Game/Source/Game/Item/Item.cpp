// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	PhysicsBox = CreateDefaultSubobject<UBoxComponent>("PhysicsBox");
	RootComponent = PhysicsBox;

	PhysicsBox->SetGenerateOverlapEvents(false);
	PhysicsBox->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PhysicsBox->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	PhysicsBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PhysicsBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	PhysicsBox->SetSimulatePhysics(true);

	// Detail의 Physics의 Constraints의 Lock Rotaion 활성화: 물리회전을 고정
	PhysicsBox->BodyInstance.bLockXRotation = true;
	PhysicsBox->BodyInstance.bLockYRotation = true;
	PhysicsBox->BodyInstance.bLockZRotation = true;


	InteractionRange = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRange"));
	InteractionRange->SetupAttachment(RootComponent);

	InteractionRange->SetGenerateOverlapEvents(true);
	InteractionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//InteractionRange->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin_InteractionRange);
	//InteractionRange->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd_InteractionRange);


	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetGenerateOverlapEvents(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AItem : Start ***/
void AItem::InitItem()
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void AItem::InitInteractionRange(float Radius)
{
	RadiusOfInteractionRange = (Radius >= 0) ? Radius : 0;

	InteractionRange->SetSphereRadius(RadiusOfInteractionRange);
}

void AItem::InitPhysicsBox(float HalfHeight)
{
	HalfHeightOfPhysicsBox = (HalfHeight >= 0) ? HalfHeight : 0;

	PhysicsBox->SetBoxExtent(FVector(RadiusOfInteractionRange / 2.0f, RadiusOfInteractionRange / 2.0f, HalfHeightOfPhysicsBox), false);
	PhysicsBox->SetRelativeLocation(FVector(0.0f, 0.0f, HalfHeightOfPhysicsBox));
}

void AItem::InitItemMesh(const TCHAR* ReferencePath, FRotator Rotation, FVector Location)
{
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(ReferencePath);
	if (staticMesh.Succeeded())
	{
		ItemMesh->SetStaticMesh(staticMesh.Object);

		// StaticMesh의 원본 사이즈 측정
		FVector minBounds, maxBounds;
		ItemMesh->GetLocalBounds(minBounds, maxBounds);

		// ItemMesh의 사이즈를 통일하기 위해 메시의 최대 사이즈로 구한 scale을 일괄적으로 적용
		float gap[3] = { maxBounds.X - minBounds.X, maxBounds.Y - minBounds.Y, maxBounds.Z - minBounds.Z };
		float maxOfGap = 0.0f;
		for (float g : gap)
		{
			if (g > maxOfGap)
				maxOfGap = g;
		}
		float scaleOfItem = (maxOfGap != 0.0f) ? (RadiusOfInteractionRange / maxOfGap) : 1.0f;
		FVector Scale(scaleOfItem, scaleOfItem, scaleOfItem);

		//FRotator Rotation(-45.0f, 0.0f, 0.0f); // 플레이어에게 잘 보이도록 45도 기울임
		//FVector Location(0.0f, 0.0f, 0.0f);

		// RootComponent인 SphereComponent가 StaticMesh의 하단 정중앙으로 오게끔 설정해줘야 함.
		// 순서는 S->R->T 순으로 해야 원점에서 벗어나지 않음.
		ItemMesh->SetRelativeScale3D(Scale);
		ItemMesh->SetRelativeRotation(Rotation);
		FVector center;
		center.X = -1.0f * (((maxBounds.X + minBounds.X) * Scale.X) / 2.0f);
		center.Y = -1.0f * (((maxBounds.Y + minBounds.Y) * Scale.Y) / 2.0f);
		center.Z = -1.0f * (minBounds.Z * Scale.Z);
		ItemMesh->SetRelativeLocation(center + Location);
	}
}

void AItem::Droped()
{
	State = EItemState::Droped;

	if (PhysicsBox)
		PhysicsBox->SetSimulatePhysics(true);

	if (InteractionRange)
		InteractionRange->SetGenerateOverlapEvents(true);

	if (ItemMesh)
		ItemMesh->SetHiddenInGame(false);
}
void AItem::Acquired()
{
	State = EItemState::Acquired;

	if (PhysicsBox)
		PhysicsBox->SetSimulatePhysics(false);

	if (InteractionRange)
		InteractionRange->SetGenerateOverlapEvents(false);

	if (ItemMesh)
		ItemMesh->SetHiddenInGame(true);
}
/*** AItem : End ***/


//void AItem::OnOverlapBegin_InteractionRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	// Other Actor is the actor that triggered the event. Check that is not ourself.  
//	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
//		return;
//
//	// Collision의 기본인 ATriggerVolume은 무시합니다.
//	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
//		return;
//
//
//	if (OtherActor->IsA(APioneer::StaticClass()))
//	{
//		if (APioneer* pioneer = Cast<APioneer>(OtherActor))
//		{
//			// APioneer의 CapsuleComponent와 충돌했을 때만
//			if (pioneer->GetCapsuleComponent() == OtherComp)
//			{
//				
//			}
//		}
//
//	}
//}
//
//void AItem::OnOverlapEnd_InteractionRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	// Other Actor is the actor that triggered the event. Check that is not ourself.  
//	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
//		return;
//
//	// Collision의 기본인 ATriggerVolume은 무시합니다.
//	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
//		return;
//
//	if (OtherActor->IsA(APioneer::StaticClass()))
//	{
//		if (APioneer* pioneer = Cast<APioneer>(OtherActor))
//		{
//			// APioneer의 CapsuleComponent와 충돌했을 때만
//			if (pioneer->GetCapsuleComponent() == OtherComp)
//			{
//				
//			}
//		}
//
//	}
//}