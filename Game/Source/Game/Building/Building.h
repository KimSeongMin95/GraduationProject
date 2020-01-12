// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Engine/TriggerVolume.h" // For ATriggerVolume::StaticClass()
/*** �𸮾��� ��� ���� : End ***/

/*** Interface ��� ���� : Start ***/
#include "Interface/HealthPointBarInterface.h"
/*** Interface ��� ���� : Start ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

UENUM()
enum class EBuildingState : uint8
{
	Constructable = 0,	/** �Ǽ��� �� �ִ��� Ȯ���ϴ� ���� */
	Constructing = 1,	/** �Ǽ����� ���� */
	Constructed = 2,	/** �Ǽ��� �Ϸ�� ����*/
	Destroying = 3		/** ������� 0���ϰ� �Ǿ� �Ҹ�Ǵ� ���� */
};

USTRUCT()
struct FTArrayOfUMaterialInterface /** TArray�� 1������ ��밡���ϹǷ� ����ü�� �̿��Ͽ� 2�������� ����մϴ�.*/
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
		TArray<class UMaterialInterface*> Object;
};

UCLASS()
class GAME_API ABuilding : public AActor, public IHealthPointBarInterface
{
	GENERATED_BODY()

	/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	ABuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/*** Basic Function : End ***/

	/*** RootComponent : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class USphereComponent* SphereComponent = nullptr; /** RootComponent */

	void InitRootComponent();
	/*** RootComponent : End ***/

public:
	EBuildingState BuildingState;

	/*** Stat : Start ***/
public:
	UPROPERTY(EditAnywhere, Category = "Stat")
		float HealthPoint; /** �ʱ� ����� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaxHealthPoint; /** �ϼ��� ����� */

	UPROPERTY(EditAnywhere, Category = "Stat")
		FVector2D Size; /** ũ�� (NxN) */

	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConstructionTime; /** �Ǽ��ð� (s) */

	UPROPERTY(EditAnywhere, Category = "Stat")
		float NeedMineral; /** �Ǽ���� ���⹰ (kg) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float NeedOrganicMatter; /** �Ǽ���� ���⹰ (kg) */

	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConsumeMineral; /** �Һ� ���⹰ (kg/h) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConsumeOrganicMatter; /** �Һ� ���⹰ (kg/h) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConsumeElectricPower; /** �Һ� ���� (MW) */

	UPROPERTY(EditAnywhere, Category = "Stat")
		float ProductionMineral; /** ���� ���⹰ (kg/h) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ProductionOrganicMatter; /** ���� ���⹰ (kg/h) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ProductionElectricPower; /** ���� ���� (MW) */

	virtual void InitStat();

	UFUNCTION(Category = "Stat")
		virtual void Calculatehealth(float Delta);
	/*** Stat : End ***/

	/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() override;
	virtual void BeginPlayHelthPointBar() final;
	virtual void TickHelthPointBar() final;
	/*** IHealthPointBarInterface : End ***/

	/*** ConstructBuildingStaticMeshComponent : Start ***/
public:
	// StaticMeshs
	UPROPERTY(VisibleAnywhere)
		TArray<class UStaticMeshComponent*> ConstructBuildingSMCs;

	// SkeltalMeshs
	UPROPERTY(VisibleAnywhere)
		TArray<class UStaticMeshComponent*> ConstructBuildingSkMCs;

	UFUNCTION()
		virtual void OnOverlapBegin_ConstructBuilding(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void InitConstructBuilding();
	void AddConstructBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind = TEXT("NULL"), FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	/*** ConstructBuildingStaticMeshComponent : End ***/

	/*** BuildingStaticMeshComponent : Start ***/
public:
	// StaticMeshs
	UPROPERTY(VisibleAnywhere)
		TArray<class UStaticMeshComponent*> BuildingSMCs;
	// TArray�� 1������ ��밡���ϹǷ� ����ü�� �̿��Ͽ� 2�������� ����մϴ�.
	UPROPERTY(VisibleAnywhere)
		TArray<FTArrayOfUMaterialInterface> BuildingSMCsMaterials; /** ���� ���͸������ ���� */
	
	// SkeltalMeshs
	UPROPERTY(VisibleAnywhere)
		TArray<class USkeletalMeshComponent*> BuildingSkMCs;
	// TArray�� 1������ ��밡���ϹǷ� ����ü�� �̿��Ͽ� 2�������� ����մϴ�.
	UPROPERTY(VisibleAnywhere)
		TArray<FTArrayOfUMaterialInterface> BuildingSkMCsMaterials; /** ���� ���͸������ ���� */

	TArray<class AActor*> OverapedActors; /** �浹�� ���͵��� ��� �����ϰ� ����� ���� */
	UFUNCTION()
		virtual void OnOverlapBegin_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void InitBuilding();
	void AddBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind = TEXT("NULL"), FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void AddBuildingSkMC(USkeletalMeshComponent** SkeletalMeshComp, UStaticMeshComponent** SubStaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind = TEXT("NULL"), FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void SetBuildingMaterials();
	/*** BuildingStaticMeshComponent : End ***/

	/*** Material : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class UMaterial* ConstructableMaterial = nullptr; /** ���λ� �������� ���͸��� */
	void SetConstructableMaterial();

	UPROPERTY(VisibleAnywhere)
		class UMaterial* UnConstructableMaterial = nullptr; /** ������ �������� ���͸��� */
	void SetUnConstructableMaterial();

	void InitMaterial();
	/*** Material : End ***/

	/*** Rotation : Start ***/
public:
	void Rotating(float Value);
	/*** Rotation : End ***/

	/*** Constructing And Destorying : Start ***/
public:
	bool Constructing();
	void Destroying();
	UFUNCTION()
		void CompleteConstructing();
	/*** Constructing And Destorying: End ***/


};
