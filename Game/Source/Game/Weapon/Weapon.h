// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** ���� ������ ��� ���� : Start ***/
#include "MyGameModeBase.h"
/*** ���� ������ ��� ���� : End ***/

/*** �𸮾��� ��� ���� : Start ***/
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
#include "Components/ArrowComponent.h"
#include "Engine.h"
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

// DECLARE_DELEGATE(FFireDelegate);

UCLASS()
class GAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

public:
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
		class UArrowComponent* ProjectileSpawnPoint;

/*** ���� ���� ���� : Start ***/
public:
	UPROPERTY(EditAnywhere, Category = "StateMent")
		int AttackPower = 0; // ���ݷ�
	UPROPERTY(EditAnywhere, Category = "StateMent")
		float AttackSpeed = 1.0f; // ���ݼӵ�
	UPROPERTY(EditAnywhere, Category = "StateMent")
		float AttackRange = 1.0f; // �����Ÿ�
	UPROPERTY(EditAnywhere, Category = "StateMent")
		int LimitedLevel = 1; // ���� ������

	UPROPERTY(EditAnywhere, Category = "StateMent")
		float FireCoolTime = 0.0f;
/*** ���� ���� ���� : End ***/

public:
	UFUNCTION()
		virtual void Fire();

/*** �������Ʈ�� Event : Start ***/
public:
	//// BlueprintImplementableEvent: ������� �������༭ �������Ʈ���� �����ϴ� ��ũ��
	//UFUNCTION(BlueprintImplementableEvent)
	//	void PullTrigger();

	// BlueprintNaitveEvent: C++���� �����ϴ� �̺�Ʈ
	/*UFUNCTION(BlueprintNaitveEvent)
	void PullTrigger();*/
/*** �������Ʈ�� Event : End ***/

	// FFireDelegate FireDelegate;

};
