// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
/*** 언리얼엔진 헤더 선언 : End ***/

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
		float RadiusOfItem;  /** 반지름 */

	UPROPERTY(EditAnywhere)
		class USphereComponent* InteractionRange = nullptr;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* StaticMeshOfItem = nullptr; /** 크기가 일정해야 함 */

	virtual void InitItem(); /** 초기화 */

	virtual void Droped();	 /** 땅에 떨어진 상태 */
	virtual void Acquired(); /** 획득된 상태 */

};
