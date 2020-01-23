// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.

#include "Engine/TriggerVolume.h" // For ATriggerVolume::StaticClass()

#include "ParticleHelper.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM()
enum class EItemState : uint8
{
	Droped = 0,  /** 바닥에 놓여진 상태 */
	Acquired = 1 /** 획득된 상태 */
};

UCLASS()
class GAME_API AItem : public AActor
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:	
	AItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AItem : Start ***/
public:
	EItemState State;

private:
	UPROPERTY(EditAnywhere, Category = "Item")
		/** 다른 액터가 이 아이템과 상호작용할 수 있는 충돌범위 */
		class USphereComponent* InteractionRange = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Item")
		/** InteractionRange의 반지름 */
		float RadiusOfInteractionRange;

	UPROPERTY(EditAnywhere, Category = "Item")
		/** Item을 Landscape 위에 올려지도록 물리가 작용하는 박스 */
		class UBoxComponent* PhysicsBox = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Item")
		/** PhysicsBox의 높이 절반 */
		float HalfHeightOfPhysicsBox;

	UPROPERTY(EditAnywhere, Category = "Item")
		/** Item으로 보여질 StaticMesh입니다. (최대 크기에 제한이 있습니다.) */
		class UStaticMeshComponent* ItemMesh = nullptr;

protected:
	virtual void InitItem(); /** 초기화 */
	void InitInteractionRange(float Radius);
	void InitPhysicsBox(float HalfHeight);
	void InitItemMesh(const TCHAR* ReferencePath, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

public:
	FORCEINLINE class USphereComponent* GetInteractionRange() const { return InteractionRange; }
	FORCEINLINE class UBoxComponent* GetPhysicsBox() const { return PhysicsBox; }
	FORCEINLINE class UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }

	virtual void Droped();	 /** 땅에 떨어진 상태 */
	virtual void Acquired(); /** 획득된 상태 */
/*** AItem : End ***/



};
