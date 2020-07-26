// Fill out your copyright notice in the Description page of Project Settings.

#include "MyDialog.h"

#include "Etc/MyTriggerBox.h"
#include "Widget/DialogWidget.h"
#include "GameMode/InGameMode.h"

AMyDialog::AMyDialog()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = nullptr;

	TimerOfDelay = 0.0f;
	TimeOfDelay = 0.0f;
	TimeOfDuration = 1.0f;
}
AMyDialog::~AMyDialog()
{

}

void AMyDialog::BeginPlay()
{
	Super::BeginPlay();
}
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
		UE_LOG(LogTemp, Error, TEXT("<AMyDialog::ShowDialog()> if (!world)"));
		return;
	}

	FString allTexts;

	for (auto& ref : Texts)
	{
		allTexts.Append(ref);
		allTexts.Append(LINE_TERMINATOR);
	}

	if (AInGameMode* inGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(world)))
	{
		inGameMode->SetTextOfDialogWidget(FText::FromString(allTexts));
		inGameMode->ActivateDialogWidget(TimeOfDuration);
	}

	Destroy();
}