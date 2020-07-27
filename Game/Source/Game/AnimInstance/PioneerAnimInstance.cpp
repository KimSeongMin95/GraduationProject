// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

#include "Network/NetworkComponent/Console.h"
#include "Character/Pioneer.h"

UPioneerAnimInstance::UPioneerAnimInstance()
{
	Pioneer = nullptr;

	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;
	bFired = false;

	Bone_Spine_01_Rotation = FRotator::ZeroRotator;
}
UPioneerAnimInstance::~UPioneerAnimInstance()
{

}

void UPioneerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (BaseCharacter)
	{
		if (BaseCharacter->IsA(APioneer::StaticClass()))
			Pioneer = Cast<APioneer>(BaseCharacter);
	}
}
void UPioneerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!TryGetPawnOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("<UPioneerAnimInstance::NativeUpdateAnimation(...)> if (!TryGetPawnOwner())"));
		return;
	}
	if (!Pioneer)
	{
		if (APawn* Owner = TryGetPawnOwner())
		{
			// Owner가 APioneer이거나 APioneer의 하위클래스인지 확인합니다.
			if (Owner->IsA(APioneer::StaticClass()))
				Pioneer = Cast<APioneer>(Owner);
		}

		UE_LOG(LogTemp, Error, TEXT("<UPioneerAnimInstance::NativeUpdateAnimation(...)> if (!Pioneer)"));
		return;
	}

	bHasPistolType = Pioneer->HasPistolType();
	bHasRifleType = Pioneer->HasRifleType();
	bHasLauncherType = Pioneer->HasLauncherType();

	bFired = Pioneer->bFired;

	Bone_Spine_01_Rotation = Pioneer->Bone_Spine_01_Rotation;
}

void UPioneerAnimInstance::FireEnd()
{
	if (!TryGetPawnOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("<UPioneerAnimInstance::FireEnd()> if (!TryGetPawnOwner())"));
		return;
	}
	if (!Pioneer)
	{
		UE_LOG(LogTemp, Warning, TEXT("<UPioneerAnimInstance::FireEnd()> if (!Pioneer)"));
		return;
	}

	Pioneer->bFired = false;
}

void UPioneerAnimInstance::DestroyCharacter()
{
	if (!TryGetPawnOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("<UPioneerAnimInstance::DestroyCharacter()> if (!TryGetPawnOwner())"));
		return;
	}
	if (!Pioneer)
	{
		UE_LOG(LogTemp, Warning, TEXT("<UPioneerAnimInstance::DestroyCharacter()> if (!Pioneer)"));
		return;
	}

	Pioneer->DestroyCharacter();
}