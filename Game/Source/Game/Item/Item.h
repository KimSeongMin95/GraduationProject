// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.

#include "Engine/TriggerVolume.h" // For ATriggerVolume::StaticClass()

#include "ParticleHelper.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
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

	/*** Item : Start ***/
public:
	EItemState State;

	virtual void InitItem(); /** �ʱ�ȭ */
	void InitStaticMeshOfItem(const TCHAR* ObjectToFind = TEXT("NULL"), FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

	virtual void Droped();	 /** ���� ������ ���� */
	virtual void Acquired(); /** ȹ��� ���� */
	/*** Item : End ***/

	/*** Physics : Start ***/
private:
	UPROPERTY(EditAnywhere)
		class UBoxComponent* PhysicsBoxComp = nullptr;
	UPROPERTY(VisibleAnywhere)
		float HalfHeightOfPhysicsBox; /** PhysicsBox�� ���� ���� */
	/*** Physics : End ***/

	/*** InteractionRange : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class USphereComponent* InteractionRange = nullptr;
	UPROPERTY(VisibleAnywhere)
		float RadiusOfInteractionRange;  /** ������ */
	//UFUNCTION()
	//	virtual void OnOverlapBegin_InteractionRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//UFUNCTION()
	//	virtual void OnOverlapEnd_InteractionRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/*** InteractionRange : End ***/

	/*** StaticMeshOfItem : Start ***/
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* StaticMeshOfItem = nullptr; /** ũ�Ⱑ �����ؾ� �� */
	/*** StaticMeshOfItem : End ***/

};
