// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDialog.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Etc/MyTriggerBox.h"

#include "CustomWidget/DialogWidget.h"

#include "GameMode/TutorialGameMode.h"
#include "GameMode/OnlineGameMode.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


// Sets default values
AMyDialog::AMyDialog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = nullptr;

	TimerOfDelay = 0.0f;
	TimeOfDelay = 0.0f;
	TimeOfDuration = 1.0f;
}

// Called when the game starts or when spawned
void AMyDialog::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyDialog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickOfDialog(DeltaTime);
}

void AMyDialog::TickOfDialog(float DeltaTime)
{
	if (!TriggerBox)
		return;

	// 트리거가 발동되지 않았다면 실행하지 않습니다.
	if (!TriggerBox->IsTriggered())
		return;

	TimerOfDelay += DeltaTime;
	if (TimerOfDelay < TimeOfDelay)
		return;

	ShowDialog();
}

void AMyDialog::ShowDialog()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMyDialog::ShowDialog()> if (!world)"));
#endif		
		return;
	}


	FString allTexts;

	for (auto& ref : Texts)
	{
		allTexts.Append(ref);
		allTexts.Append(LINE_TERMINATOR);
	}

	ATutorialGameMode* tutorialGameMode = Cast<ATutorialGameMode>(UGameplayStatics::GetGameMode(world));
	AOnlineGameMode* onlineGameMode = Cast<AOnlineGameMode>(UGameplayStatics::GetGameMode(world));

	if (tutorialGameMode)
	{
		tutorialGameMode->SetTextOfDialogWidget(FText::FromString(allTexts));
		tutorialGameMode->ActivateDialogWidget(TimeOfDuration);

	}
	else if (onlineGameMode)
	{
		onlineGameMode->SetTextOfDialogWidget(FText::FromString(allTexts));
		onlineGameMode->ActivateDialogWidget(TimeOfDuration);
	}

	Destroy();
}