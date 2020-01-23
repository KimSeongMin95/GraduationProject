// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Pioneer.h"
/*** ���� ������ ��� ���� ���� : End ***/


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

	// Detail�� Physics�� Constraints�� Lock Rotaion Ȱ��ȭ: ����ȸ���� ����
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
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
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

		// StaticMesh�� ���� ������ ����
		FVector minBounds, maxBounds;
		ItemMesh->GetLocalBounds(minBounds, maxBounds);

		// ItemMesh�� ����� �����ϱ� ���� �޽��� �ִ� ������� ���� scale�� �ϰ������� ����
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