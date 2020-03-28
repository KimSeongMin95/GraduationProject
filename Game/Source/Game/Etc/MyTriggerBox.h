// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "MyTriggerBox.generated.h"


UCLASS()
class GAME_API AMyTriggerBox : public ATriggerBox
{
	GENERATED_BODY()


/*** Basic Function : Start ***/
public:
	AMyTriggerBox();

protected:
	virtual void BeginPlay() override;
/*** Basic Function : End ***/


/*** AMyTriggerBox : Start ***/
public:
	UPROPERTY(EditAnywhere, Category = "AMyTriggerBox")
		bool bIsTriggered;

protected:
	UFUNCTION(Category = "OnOverlapBegin")
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(Category = "AMyTriggerBox")
		bool IsTriggered();
	
/*** AMyTriggerBox : End ***/
};
