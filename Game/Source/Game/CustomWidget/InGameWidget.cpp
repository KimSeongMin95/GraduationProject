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

	if (!WidgetTree)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::InitWidget(...)> if (!WidgetTree) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::InitWidget(...)> if (!WidgetTree)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetArrowButtonsVisibility(...)> if (!LeftArrowButton || !RightArrowButton) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetArrowButtonsVisibility(...)> if (!LeftArrowButton || !RightArrowButton)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetPossessButtonVisibility(...)> if (!PossessButton) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetPossessButtonVisibility(...)> if (!PossessButton)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetFreeViewpointButtonVisibility(...)> if (!FreeViewpointButton) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetFreeViewpointButtonVisibility(...)> if (!FreeViewpointButton)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetObservingButtonVisibility(...)> if (!ObservingButton) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetObservingButtonVisibility(...)> if (!ObservingButton)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetTextOfResources(...)> if (!NumOfPioneer || !NumOfMineral || !NumOfOrganic || !NumOfEnergy) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfResources(...)> if (!NumOfPioneer || !NumOfMineral || !NumOfOrganic || !NumOfEnergy)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetBuildingBoxVisibility(...)> if (!BuildingBox) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetBuildingBoxVisibility(...)> if (!BuildingBox)"));
#endif				
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetPioneerBoxVisibility(...)> if (!PioneerBox) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetPioneerBoxVisibility(...)> if (!PioneerBox)"));
#endif				
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetTextOfPioneerBox(...)> if (!Pioneer) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfPioneerBox(...)> if (!Pioneer)"));
#endif			
		return;
	}

	if (!HealthOfPioneer || !IDOfPioneer || !MoveSpeedOfPioneer || !AttackSpeedOfPioneer ||
		!AttackPowerOfPioneer || !SightRangeOfPioneer || !DetectRangeOfPioneer || !AttackRangeOfPioneer)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetTextOfPioneerBox(...)> if (!HealthOfPioneer || !IDOfPioneer || !MoveSpeedOfPioneer || !AttackSpeedOfPioneer ||!AttackPowerOfPioneer || !SightRangeOfPioneer || !DetectRangeOfPioneer || !AttackRangeOfPioneer) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfPioneerBox(...)> if (!HealthOfPioneer || !IDOfPioneer || !MoveSpeedOfPioneer || !AttackSpeedOfPioneer ||!AttackPowerOfPioneer || !SightRangeOfPioneer || !DetectRangeOfPioneer || !AttackRangeOfPioneer)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetWeaponBoxVisibility(...)> if (!WeaponBox) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetWeaponBoxVisibility(...)> if (!WeaponBox)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetTextOfWeaponBox(...)> if (!Weapon) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfWeaponBox(...)> if (!Weapon)"));
#endif			
		return;
	}

	if (!ImageOfPistol || !ImageOfAssaultRifle || !ImageOfShotgun || !ImageOfSniperRifle || !ImageOfGrenadeLauncher || !ImageOfRocketLauncher)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetTextOfWeaponBox(...)> if (!ImageOfPistol || !ImageOfAssaultRifle || !ImageOfShotgun || !ImageOfSniperRifle || !ImageOfGrenadeLauncher || !ImageOfRocketLauncher) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfWeaponBox(...)> if (!ImageOfPistol || !ImageOfAssaultRifle || !ImageOfShotgun || !ImageOfSniperRifle || !ImageOfGrenadeLauncher || !ImageOfRocketLauncher)"));
#endif			
		return;
	}

	if (!LimitedLevelOfWeapon || !AttackPowerOfWeapon || !AttackSpeedOfWeapon || !AttackRangeOfWeapon || !ReloadTimeOfWeapon)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UInGameWidget::SetTextOfWeaponBox(...)> if (!LimitedLevelOfWeapon || !AttackPowerOfWeapon || !AttackSpeedOfWeapon || !AttackRangeOfWeapon || !ReloadTimeOfWeapon) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfWeaponBox(...)> if (!LimitedLevelOfWeapon || !AttackPowerOfWeapon || !AttackSpeedOfWeapon || !AttackRangeOfWeapon || !ReloadTimeOfWeapon)"));
#endif			
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