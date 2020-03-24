// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManager.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Pioneer.h"

#include "Item/Weapon/Weapon.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AWeaponManager::AWeaponManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;
}

void AWeaponManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


