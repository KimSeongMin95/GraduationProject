// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Pioneer.h"
/*** ���� ������ ��� ���� ���� : End ***/

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



//void AItem::OnOverlapBegin_InteractionRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	// Other Actor is the actor that triggered the event. Check that is not ourself.  
//	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
//		return;
//
//	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
//	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
//		return;
//
//
//	if (OtherActor->IsA(APioneer::StaticClass()))
//	{
//		if (APioneer* pioneer = Cast<APioneer>(OtherActor))
//		{
//			// APioneer�� CapsuleComponent�� �浹���� ����
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
//	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
//	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
//		return;
//
//	if (OtherActor->IsA(APioneer::StaticClass()))
//	{
//		if (APioneer* pioneer = Cast<APioneer>(OtherActor))
//		{
//			// APioneer�� CapsuleComponent�� �浹���� ����
//			if (pioneer->GetCapsuleComponent() == OtherComp)
//			{
//				
//			}
//		}
//
//	}
//}

void AItem::InitItem()
{
	State = EItemState::Droped;

	HalfHeightOfPhysicsBox = 5.0f; // �ʹ� ������ �浹�� ����� �̷������ ����.

	RadiusOfInteractionRange = 192.0f;

	PhysicsBoxComp = CreateDefaultSubobject<UBoxComponent>("PhysicsBoxComp");
	RootComponent = PhysicsBoxComp;
	PhysicsBoxComp->SetBoxExtent(FVector(RadiusOfInteractionRange / 2.0f, RadiusOfInteractionRange / 2.0f, HalfHeightOfPhysicsBox), false);
	PhysicsBoxComp->SetGenerateOverlapEvents(false);
	PhysicsBoxComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PhysicsBoxComp->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	PhysicsBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PhysicsBoxComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	PhysicsBoxComp->SetSimulatePhysics(true);
	PhysicsBoxComp->SetRelativeLocation(FVector(0.0f, 0.0f, HalfHeightOfPhysicsBox));

	// Detail�� Physics�� Constraints�� Lock Rotaion Ȱ��ȭ: ����ȸ���� ����
	PhysicsBoxComp->BodyInstance.bLockXRotation = true;
	PhysicsBoxComp->BodyInstance.bLockYRotation = true;
	PhysicsBoxComp->BodyInstance.bLockZRotation = true;

	InteractionRange = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRange"));
	InteractionRange->SetupAttachment(RootComponent);
	InteractionRange->SetSphereRadius(RadiusOfInteractionRange);
	InteractionRange->SetGenerateOverlapEvents(true);
	InteractionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//InteractionRange->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin_InteractionRange);
	//InteractionRange->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd_InteractionRange);

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

		// StaticMesh�� ���� ������ ����
		FVector minBounds, maxBounds;
		StaticMeshOfItem->GetLocalBounds(minBounds, maxBounds);

		// StaticMeshOfItem�� ����� �����ϱ� ���� �޽��� �ִ� ������� ���� scale�� �ϰ������� ����
		float gap[3] = { maxBounds.X - minBounds.X, maxBounds.Y - minBounds.Y, maxBounds.Z - minBounds.Z };
		float maxOfGap = 0.0f;
		for (float g : gap)
		{
			if (g > maxOfGap)
				maxOfGap = g;
		}
		float scaleOfItem = (maxOfGap != 0.0f) ? (RadiusOfInteractionRange / maxOfGap) : 1.0f;
		FVector Scale(scaleOfItem, scaleOfItem, scaleOfItem);

		//FRotator Rotation(-45.0f, 0.0f, 0.0f); // �÷��̾�� �� ���̵��� 45�� �����
		//FVector Location(0.0f, 0.0f, 0.0f);

		// RootComponent�� SphereComponent�� StaticMesh�� �ϴ� ���߾����� ���Բ� ��������� ��.
		// ������ S->R->T ������ �ؾ� �������� ����� ����.
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