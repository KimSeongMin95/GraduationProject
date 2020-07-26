// Fill out your copyright notice in the Description page of Project Settings.

#include "MyTriggerBox.h"

#include "Character/Pioneer.h"

AMyTriggerBox::AMyTriggerBox()
{
	if (GetCollisionComponent())
	{
		GetCollisionComponent()->SetGenerateOverlapEvents(true);
		GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCollisionComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
		GetCollisionComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetCollisionComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyTriggerBox::OnOverlapBegin);
	}

	bIsTriggered = false;
}
AMyTriggerBox::~AMyTriggerBox()
{

}

void AMyTriggerBox::BeginPlay()
{
	Super::BeginPlay();

}

void AMyTriggerBox::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;
	if (OtherActor == this)
		return;
	/**************************************************/

	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			if (pioneer->GetCapsuleComponent() == OtherComp)
			{
				bIsTriggered = true;

				GetCollisionComponent()->SetGenerateOverlapEvents(false);
				GetCollisionComponent()->OnComponentBeginOverlap.Clear();
			}
		}
	}
}

bool AMyTriggerBox::IsTriggered()
{
	return bIsTriggered;
}
