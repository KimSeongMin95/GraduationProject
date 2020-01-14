// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM()
enum class EItemState : uint8
{
	Droped = 0,
	Acquired = 1
};

UCLASS()
class GAME_API AItem : public AActor
{
	GENERATED_BODY()

	/*** Basic Function : Start ***/
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/*** Basic Function : End ***/

public:
	EItemState State;

	UPROPERTY(VisibleAnywhere)
		float RadiusOfItem;  /** ������ */

	UPROPERTY(EditAnywhere)
		class USphereComponent* InteractionRange = nullptr;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* StaticMeshOfItem = nullptr; /** ũ�Ⱑ �����ؾ� �� */

	virtual void InitItem(); /** �ʱ�ȭ */

	virtual void Droped();	 /** ���� ������ ���� */
	virtual void Acquired(); /** ȹ��� ���� */

};
