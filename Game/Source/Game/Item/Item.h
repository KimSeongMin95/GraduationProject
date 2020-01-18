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

	/*** Item : Start ***/
public:
	EItemState State;

	virtual void InitItem(); /** 초기화 */
	void InitStaticMeshOfItem(const TCHAR* ObjectToFind = TEXT("NULL"), FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

	virtual void Droped();	 /** 땅에 떨어진 상태 */
	virtual void Acquired(); /** 획득된 상태 */
	/*** Item : End ***/

	/*** Physics : Start ***/
private:
	UPROPERTY(EditAnywhere)
		class UBoxComponent* PhysicsBoxComp = nullptr;
	UPROPERTY(VisibleAnywhere)
		float HalfHeightOfPhysicsBox; /** PhysicsBox의 높이 절반 */
	/*** Physics : End ***/

	/*** InteractionRange : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class USphereComponent* InteractionRange = nullptr;
	UPROPERTY(VisibleAnywhere)
		float RadiusOfInteractionRange;  /** 반지름 */
	//UFUNCTION()
	//	virtual void OnOverlapBegin_InteractionRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//UFUNCTION()
	//	virtual void OnOverlapEnd_InteractionRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/*** InteractionRange : End ***/

	/*** StaticMeshOfItem : Start ***/
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* StaticMeshOfItem = nullptr; /** 크기가 일정해야 함 */
	/*** StaticMeshOfItem : End ***/

};
