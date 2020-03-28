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
		GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyTriggerBox::OnOverlapBegin);

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
	//UE_LOG(LogTemp, Log, TEXT("OtherActor FName :: %s"), *OtherActor->GetFName().ToString());

	// Pioneer
	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// 만약 OtherActor가 APioneer이기는 하지만 APioneer의 DetectRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->GetDetectRangeSphereComp() == OtherComp || pioneer->GetAttackRangeSphereComp() == OtherComp)
				return;


			bIsTriggered = true;

			GetCollisionComponent()->SetGenerateOverlapEvents(false);
			GetCollisionComponent()->OnComponentBeginOverlap.Clear();
		}
	}
}

bool AMyTriggerBox::IsTriggered()
{
	return bIsTriggered;
}
/*** AMyTriggerBox : End ***/