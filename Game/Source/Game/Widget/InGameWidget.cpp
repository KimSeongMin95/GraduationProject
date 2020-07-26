// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameWidget.h"

#include "Character/Pioneer.h"
#include "Weapon/Weapon.h"

UInGameWidget::UInGameWidget()
{
	LeftArrowButton = nullptr;
	RightArrowButton = nullptr;

	PossessButton = nullptr;
	FreeViewpointButton = nullptr;
	ObservingButton = nullptr;

	NumOfPioneer = nullptr;
	NumOfMineral = nullptr;
	NumOfOrganic = nullptr;
	NumOfEnergy = nullptr;

	BuildingBox = nullptr;

	PioneerBox = nullptr;
	HealthOfPioneer = nullptr;
	IDOfPioneer = nullptr;
	MoveSpeedOfPioneer = nullptr;
	LevelOfPioneer = nullptr;
	ExpOfPioneer = nullptr;
	SightRangeOfPioneer = nullptr;
	DetectRangeOfPioneer = nullptr;
	AttackRangeOfPioneer = nullptr;

	WeaponBox = nullptr;
	ImageOfPistol = nullptr;
	ImageOfAssaultRifle = nullptr;
	ImageOfShotgun = nullptr;
	ImageOfSniperRifle = nullptr;
	ImageOfGrenadeLauncher = nullptr;
	ImageOfRocketLauncher = nullptr;
	LimitedLevelOfWeapon = nullptr;
	AttackPowerOfWeapon = nullptr;
	AttackSpeedOfWeapon = nullptr;
	AttackRangeOfWeapon = nullptr;
	ReloadTimeOfWeapon = nullptr;
}
UInGameWidget::~UInGameWidget()
{

}

bool UInGameWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}


	LeftArrowButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_LeftArrow")));
	RightArrowButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_RightArrow")));

	PossessButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_Possess")));
	FreeViewpointButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_FreeViewpoint")));
	ObservingButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_Observing")));

	NumOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_NumOfPioneer")));
	NumOfMineral = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_NumOfMineral")));
	NumOfOrganic = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_NumOfOrganic")));
	NumOfEnergy = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_NumOfEnergy")));

	BuildingBox = WidgetTree->FindWidget<UHorizontalBox>(FName(TEXT("HorizontalBox_BuidlingBox")));

	PioneerBox = WidgetTree->FindWidget<UHorizontalBox>(FName(TEXT("HorizontalBox_PioneerBox")));
	HealthOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_Health")));
	IDOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_ID")));
	MoveSpeedOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_MoveSpeed")));
	LevelOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_Level")));
	ExpOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_Exp")));
	SightRangeOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_SightRange")));
	DetectRangeOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_DetectRange")));
	AttackRangeOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_AttackRange")));

	WeaponBox = WidgetTree->FindWidget<UHorizontalBox>(FName(TEXT("HorizontalBox_WeaponBox")));
	ImageOfPistol = WidgetTree->FindWidget<UImage>(FName(TEXT("Image_Weapon_Pistol")));
	ImageOfAssaultRifle = WidgetTree->FindWidget<UImage>(FName(TEXT("Image_Weapon_AssaultRifle")));
	ImageOfShotgun = WidgetTree->FindWidget<UImage>(FName(TEXT("Image_Weapon_Shotgun")));
	ImageOfSniperRifle = WidgetTree->FindWidget<UImage>(FName(TEXT("Image_Weapon_SniperRifle")));
	ImageOfGrenadeLauncher = WidgetTree->FindWidget<UImage>(FName(TEXT("Image_Weapon_GrenadeLauncher")));
	ImageOfRocketLauncher = WidgetTree->FindWidget<UImage>(FName(TEXT("Image_Weapon_RocketLauncher")));
	LimitedLevelOfWeapon = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_LimitedLevel")));
	AttackPowerOfWeapon = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Weapon_AttackPower")));
	AttackSpeedOfWeapon = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Weapon_AttackSpeed")));
	AttackRangeOfWeapon = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Weapon_AttackRange")));
	ReloadTimeOfWeapon = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Weapon_ReloadTime")));

	return true;
}

void UInGameWidget::SetArrowButtonsVisibility(bool bVisible)
{
	if (!LeftArrowButton || !RightArrowButton)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetArrowButtonsVisibility(...)> if (!LeftArrowButton || !RightArrowButton)"));
		return;
	}

	if (bVisible)
	{
		LeftArrowButton->SetVisibility(ESlateVisibility::Visible);
		RightArrowButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		LeftArrowButton->SetVisibility(ESlateVisibility::Hidden);
		RightArrowButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UInGameWidget::SetPossessButtonVisibility(bool bVisible)
{
	if (!PossessButton)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetPossessButtonVisibility(...)> if (!PossessButton)"));
		return;
	}

	if (bVisible)
	{
		PossessButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PossessButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UInGameWidget::SetFreeViewpointButtonVisibility(bool bVisible)
{
	if (!FreeViewpointButton)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetFreeViewpointButtonVisibility(...)> if (!FreeViewpointButton)"));
		return;
	}

	if (bVisible)
	{
		FreeViewpointButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FreeViewpointButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UInGameWidget::SetObservingButtonVisibility(bool bVisible)
{
	if (!ObservingButton)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetObservingButtonVisibility(...)> if (!ObservingButton)"));
		return;
	}

	if (bVisible)
	{
		ObservingButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ObservingButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInGameWidget::SetTextOfResources(int nPioneer, const class cInfoOfResources& Resources)
{
	if (!NumOfPioneer || !NumOfMineral || !NumOfOrganic || !NumOfEnergy)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfResources(...)> if (!NumOfPioneer || !NumOfMineral || !NumOfOrganic || !NumOfEnergy)"));
		return;
	}

	NumOfPioneer->SetText(FText::FromString(FString::FromInt(nPioneer)));
	NumOfMineral->SetText(FText::FromString(FString::FromInt(Resources.NumOfMineral)));
	NumOfOrganic->SetText(FText::FromString(FString::FromInt(Resources.NumOfOrganic)));
	NumOfEnergy->SetText(FText::FromString(FString::FromInt(Resources.NumOfEnergy)));
}

void UInGameWidget::SetBuildingBoxVisibility(bool bVisible)
{
	if (!BuildingBox)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetBuildingBoxVisibility(...)> if (!BuildingBox)"));
		return;
	}

	if (bVisible)
	{
		BuildingBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		BuildingBox->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInGameWidget::SetPioneerBoxVisibility(bool bVisible)
{
	if (!PioneerBox)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetPioneerBoxVisibility(...)> if (!PioneerBox)"));
		return;
	}

	if (bVisible)
	{
		PioneerBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		PioneerBox->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UInGameWidget::SetTextOfPioneerBox(class APioneer* Pioneer)
{
	if (!Pioneer)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfPioneerBox(...)> if (!Pioneer)"));
		return;
	}
	if (!HealthOfPioneer || !IDOfPioneer || !MoveSpeedOfPioneer || !LevelOfPioneer ||
		!ExpOfPioneer || !SightRangeOfPioneer || !DetectRangeOfPioneer || !AttackRangeOfPioneer)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfPioneerBox(...)> if (!HealthOfPioneer || !IDOfPioneer || !MoveSpeedOfPioneer || !LevelOfPioneer ||!ExpOfPioneer || !SightRangeOfPioneer || !DetectRangeOfPioneer || !AttackRangeOfPioneer)"));
		return;
	}

	FString tHealth = FString::FromInt(Pioneer->HealthPoint) + " / " + FString::FromInt(Pioneer->MaxHealthPoint);
	HealthOfPioneer->SetText(FText::FromString(tHealth));
	
	FString tID = TEXT("ID: ") + Pioneer->NameOfID;
	IDOfPioneer->SetText(FText::FromString(tID));

	FString tMoveSpeed = TEXT("이동속도: ") + FString::SanitizeFloat(Pioneer->MoveSpeed);
	MoveSpeedOfPioneer->SetText(FText::FromString(tMoveSpeed));

	FString tLevel = TEXT("레벨: ") + FString::FromInt(Pioneer->Level);
	LevelOfPioneer->SetText(FText::FromString(tLevel));

	FString tExp = TEXT("경험치: ") + FString::SanitizeFloat(Pioneer->Exp);
	ExpOfPioneer->SetText(FText::FromString(tExp));

	FString tSightRange = TEXT("시야범위: ") + FString::SanitizeFloat(Pioneer->SightRange * 64.0f);
	SightRangeOfPioneer->SetText(FText::FromString(tSightRange));

	FString tDetectRange = TEXT("탐지범위: ") + FString::SanitizeFloat(Pioneer->DetectRange * 64.0f);
	DetectRangeOfPioneer->SetText(FText::FromString(tDetectRange));

	FString tAttackRange = TEXT("공격범위: ") + FString::SanitizeFloat(Pioneer->AttackRange * 64.0f);
	AttackRangeOfPioneer->SetText(FText::FromString(tAttackRange));
}

void UInGameWidget::SetWeaponBoxVisibility(bool bVisible)
{
	if (!WeaponBox)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetWeaponBoxVisibility(...)> if (!WeaponBox)"));
		return;
	}

	if (bVisible)
	{
		WeaponBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		WeaponBox->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UInGameWidget::SetTextOfWeaponBox(class AWeapon* Weapon)
{
	if (!Weapon)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfWeaponBox(...)> if (!Weapon)"));
		return;
	}
	if (!ImageOfPistol || !ImageOfAssaultRifle || !ImageOfShotgun || !ImageOfSniperRifle || !ImageOfGrenadeLauncher || !ImageOfRocketLauncher)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfWeaponBox(...)> if (!ImageOfPistol || !ImageOfAssaultRifle || !ImageOfShotgun || !ImageOfSniperRifle || !ImageOfGrenadeLauncher || !ImageOfRocketLauncher)"));
		return;
	}
	if (!LimitedLevelOfWeapon || !AttackPowerOfWeapon || !AttackSpeedOfWeapon || !AttackRangeOfWeapon || !ReloadTimeOfWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfWeaponBox(...)> if (!LimitedLevelOfWeapon || !AttackPowerOfWeapon || !AttackSpeedOfWeapon || !AttackRangeOfWeapon || !ReloadTimeOfWeapon)"));
		return;
	}

	
	if (ImageOfPistol->IsVisible())
		ImageOfPistol->SetVisibility(ESlateVisibility::Hidden);
	if (ImageOfAssaultRifle->IsVisible())
		ImageOfAssaultRifle->SetVisibility(ESlateVisibility::Hidden);
	if (ImageOfShotgun->IsVisible())
		ImageOfShotgun->SetVisibility(ESlateVisibility::Hidden);
	if (ImageOfSniperRifle->IsVisible())
		ImageOfSniperRifle->SetVisibility(ESlateVisibility::Hidden);
	if (ImageOfGrenadeLauncher->IsVisible())
		ImageOfGrenadeLauncher->SetVisibility(ESlateVisibility::Hidden);
	if (ImageOfRocketLauncher->IsVisible())
		ImageOfRocketLauncher->SetVisibility(ESlateVisibility::Hidden);

	switch (Weapon->WeaponNumbering)
	{
	case 1:
		ImageOfPistol->SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	case 2:
		ImageOfAssaultRifle->SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	case 3:
		ImageOfShotgun->SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	case 4:
		ImageOfSniperRifle->SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	case 5:
		ImageOfGrenadeLauncher->SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	case 6:
		ImageOfRocketLauncher->SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	}

	FString tLimitedLevel = TEXT("제한 레벨: ") + FString::FromInt(Weapon->LimitedLevel);
	LimitedLevelOfWeapon->SetText(FText::FromString(tLimitedLevel));

	FString tAttackPower = TEXT("공격력: ") + FString::SanitizeFloat(Weapon->AttackPower);
	AttackPowerOfWeapon->SetText(FText::FromString(tAttackPower));

	FString tAttackSpeed = TEXT("공격속도: ") + FString::SanitizeFloat(Weapon->AttackSpeed);
	AttackSpeedOfWeapon->SetText(FText::FromString(tAttackSpeed));

	FString tAttackRange = TEXT("공격범위: ") + FString::SanitizeFloat(Weapon->AttackRange);
	AttackRangeOfWeapon->SetText(FText::FromString(tAttackRange));
}