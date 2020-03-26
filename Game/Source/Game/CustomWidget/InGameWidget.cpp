// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameWidget.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"

#include "Item/Weapon/Weapon.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


UInGameWidget::UInGameWidget()
{
	///////////
	// 초기화
	///////////	
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
	AttackSpeedOfPioneer = nullptr;
	AttackPowerOfPioneer = nullptr;
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

	if (WidgetTree == nullptr)
	{
		printf_s("[Error] <UInGameWidget::InitWidget(...)> if (WidgetTree == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[Error] <UInGameWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
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
	AttackSpeedOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_AttackSpeed")));
	AttackPowerOfPioneer = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Pioneer_AttackPower")));
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
		printf_s("[ERROR] <UWaitingGameWidget::SetArrowButtonsVisibility(...)> if (!LeftArrowButton || !RightArrowButton) \n");
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
		printf_s("[ERROR] <UWaitingGameWidget::SetPossessButtonVisibility(...)> if (!PossessButton) \n");
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
		printf_s("[ERROR] <UWaitingGameWidget::SetFreeViewpointButtonVisibility(...)> if (!FreeViewpointButton) \n");
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
		printf_s("[ERROR] <UWaitingGameWidget::SetObservingButtonVisibility(...)> if (!ObservingButton) \n");
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
		printf_s("[ERROR] <UWaitingGameWidget::SetTextOfResources(...)> if (!NumOfPioneer || !NumOfMineral || !NumOfOrganic || !NumOfEnergy)\n");
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
		printf_s("[ERROR] <UWaitingGameWidget::SetBuildingBoxVisibility(...)> if (!BuildingBox) \n");
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
		printf_s("[ERROR] <UWaitingGameWidget::SetPioneerBoxVisibility(...)> if (!PioneerBox) \n");
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
		printf_s("[ERROR] <UWaitingGameWidget::SetTextOfPioneerBox(...)> if (!Pioneer)\n");
		return;
	}
	if (!HealthOfPioneer || !IDOfPioneer || !MoveSpeedOfPioneer || !AttackSpeedOfPioneer ||
		!AttackPowerOfPioneer || !SightRangeOfPioneer || !DetectRangeOfPioneer || !AttackRangeOfPioneer)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetTextOfPioneerBox(...)> if (!HealthOfPioneer || !IDOfPioneer || !MoveSpeedOfPioneer || !AttackSpeedOfPioneer || !AttackPowerOfPioneer || !SightRangeOfPioneer || !DetectRangeOfPioneer || !AttackRangeOfPioneer) \n");
		return;
	}

	FString tHealth = FString::SanitizeFloat(Pioneer->HealthPoint) + " / " + FString::FromInt(Pioneer->MaxHealthPoint);
	HealthOfPioneer->SetText(FText::FromString(tHealth));
	
	FString tID = "ID: " + Pioneer->NameOfID;
	IDOfPioneer->SetText(FText::FromString(tID));

	FString tMoveSpeed = "MoveSpeed: " + FString::SanitizeFloat(Pioneer->MoveSpeed);
	MoveSpeedOfPioneer->SetText(FText::FromString(tMoveSpeed));

	FString tAttackSpeed = "AttackSpeed: " + FString::SanitizeFloat(Pioneer->AttackSpeed);
	AttackSpeedOfPioneer->SetText(FText::FromString(tAttackSpeed));

	FString tAttackPower = "AttackPower: " + FString::SanitizeFloat(Pioneer->AttackPower);
	AttackPowerOfPioneer->SetText(FText::FromString(tAttackPower));

	FString tSightRange = "SightRange: " + FString::SanitizeFloat(Pioneer->SightRange);
	SightRangeOfPioneer->SetText(FText::FromString(tSightRange));

	FString tDetectRange = "DetectRange: " + FString::SanitizeFloat(Pioneer->DetectRange);
	DetectRangeOfPioneer->SetText(FText::FromString(tDetectRange));

	FString tAttackRange = "AttackRange: " + FString::SanitizeFloat(Pioneer->AttackRange);
	AttackRangeOfPioneer->SetText(FText::FromString(tAttackRange));
}

void UInGameWidget::SetWeaponBoxVisibility(bool bVisible)
{
	if (!WeaponBox)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetWeaponBoxVisibility(...)> if (!WeaponBox) \n");
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
		printf_s("[ERROR] <UWaitingGameWidget::SetTextOfWeaponBox(...)> if (!Weapon)\n");
		return;
	}
	if (!ImageOfPistol || !ImageOfAssaultRifle || !ImageOfShotgun || !ImageOfSniperRifle || !ImageOfGrenadeLauncher || !ImageOfRocketLauncher)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetTextOfWeaponBox(...)> if (!ImageOfPistol || !ImageOfAssaultRifle || !ImageOfShotgun || !ImageOfSniperRifle || !ImageOfGrenadeLauncher || !ImageOfRocketLauncher) \n");
		return;
	}
	if (!LimitedLevelOfWeapon || !AttackPowerOfWeapon || !AttackSpeedOfWeapon || !AttackRangeOfWeapon || !ReloadTimeOfWeapon)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetTextOfWeaponBox(...)>	if (!LimitedLevelOfWeapon || !AttackPowerOfWeapon || !AttackSpeedOfWeapon || !AttackRangeOfWeapon || !ReloadTimeOfWeapon) \n");
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

	FString tLimitedLevel = "Limit Lv: " + FString::FromInt(Weapon->LimitedLevel);
	LimitedLevelOfWeapon->SetText(FText::FromString(tLimitedLevel));

	FString tAttackPower = "AttackPower: " + FString::SanitizeFloat(Weapon->AttackPower);
	AttackPowerOfWeapon->SetText(FText::FromString(tAttackPower));

	FString tAttackSpeed = "AttackSpeed: " + FString::SanitizeFloat(Weapon->AttackSpeed);
	AttackSpeedOfWeapon->SetText(FText::FromString(tAttackSpeed));

	FString tAttackRange = "AttackRange: " + FString::SanitizeFloat(Weapon->AttackRange);
	AttackRangeOfWeapon->SetText(FText::FromString(tAttackRange));

	FString tReloadTime = "ReloadTime: " + FString::SanitizeFloat(Weapon->ReloadTime, 2);
	ReloadTimeOfWeapon->SetText(FText::FromString(tReloadTime));

}