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