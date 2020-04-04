// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTriggerBox.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"

#include "Character/Pioneer.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
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

void AMyTriggerBox::BeginPlay()
{
	Super::BeginPlay();


}
/*** Basic Function : End ***/


/*** AMyTriggerBox : Start ***/
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
/*** AMyTriggerBox : End ***/