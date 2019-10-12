// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
		class USkeletalMeshComponent* PistolMesh;

	UPROPERTY(EditAnywhere)
		class UArrowComponent* ProjectileSpawnPoint;

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
