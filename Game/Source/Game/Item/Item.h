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
	Droped = 0,  /** �ٴڿ� ������ ���� */
	Acquired = 1 /** ȹ��� ���� */
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
		/** �ٸ� ���Ͱ� �� �����۰� ��ȣ�ۿ��� �� �ִ� �浹���� */
		class USphereComponent* InteractionRange = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Item")
		/** InteractionRange�� ������ */
		float RadiusOfInteractionRange;

	UPROPERTY(EditAnywhere, Category = "Item")
		/** Item�� Landscape ���� �÷������� ������ �ۿ��ϴ� �ڽ� */
		class UBoxComponent* PhysicsBox = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Item")
		/** PhysicsBox�� ���� ���� */
		float HalfHeightOfPhysicsBox;

	UPROPERTY(EditAnywhere, Category = "Item")
		/** Item���� ������ StaticMesh�Դϴ�. (�ִ� ũ�⿡ ������ �ֽ��ϴ�.) */
		class UStaticMeshComponent* ItemMesh = nullptr;

protected:
	virtual void InitItem(); /** �ʱ�ȭ */
	void InitInteractionRange(float Radius);
	void InitPhysicsBox(float HalfHeight);
	void InitItemMesh(const TCHAR* ReferencePath, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

public:
	FORCEINLINE class USphereComponent* GetInteractionRange() const { return InteractionRange; }
	FORCEINLINE class UBoxComponent* GetPhysicsBox() const { return PhysicsBox; }
	FORCEINLINE class UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }

	virtual void Droped();	 /** ���� ������ ���� */
	virtual void Acquired(); /** ȹ��� ���� */
/*** AItem : End ***/



};
