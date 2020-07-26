// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Engine/TriggerVolume.h" // For ATriggerVolume::StaticClass()

#include "Interface/HealthPointBarInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

UENUM()
enum class EBuildingType : uint8
{
	Wall,
	Stairs,
	Gate,
	InorganicMine,
	OrganicMine,
	NuclearFusionPowerPlant,
	AssaultRifleTurret,
	SniperRifleTurret,
	RocketLauncherTurret
};

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

public:
	ABuilding();
	virtual ~ABuilding();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "ABuilding")
		class USphereComponent* SphereComponent = nullptr; 

protected:
	UPROPERTY(VisibleAnywhere, Category = "ConstructBuilding")
		TArray<class UStaticMeshComponent*> ConstructBuildingSMCs; /** StaticMeshs */
	UPROPERTY(VisibleAnywhere, Category = "ConstructBuilding")
		TArray<class UStaticMeshComponent*> ConstructBuildingSkMCs; /** SkeltalMeshs */
	UPROPERTY(VisibleAnywhere, Category = "Building")
		TArray<class UStaticMeshComponent*> BuildingSMCs; /** StaticMeshs */
	UPROPERTY(VisibleAnywhere, Category = "Building")
		TArray<class USkeletalMeshComponent*> BuildingSkMCs; /** SkeltalMeshs */

	UPROPERTY(VisibleAnywhere)
		TArray<class AActor*> OverlappedActors; /** �浹�� ���͵��� ��� �����ϰ� ����� �����մϴ�. */

	UPROPERTY(VisibleAnywhere, Category = "Building")
		TArray<FTArrayOfUMaterialInterface> BuildingSMCsMaterials; /** StaticMeshs�� ���� ���͸������ �����մϴ�. */
	UPROPERTY(VisibleAnywhere, Category = "Building")
		TArray<FTArrayOfUMaterialInterface> BuildingSkMCsMaterials; /** SkeltalMeshs�� ���� ���͸������ �����մϴ�. */

	UPROPERTY(VisibleAnywhere)
		class UMaterial* ConstructableMaterial = nullptr; /** EBuildingState::Constructable�� �� ����� ���λ� �������� ���͸��� */
	UPROPERTY(VisibleAnywhere)
		class UMaterial* UnConstructableMaterial = nullptr; /** EBuildingState::Constructable�� �� ����� ������ �������� ���͸��� */
	
	FTimerHandle TimerOfConstructing;

public:
	UPROPERTY(VisibleAnywhere, Category = "BuildingManager")
		class ABuildingManager* BuildingManager = nullptr;

	UPROPERTY(VisibleAnywhere)
		bool bDying;

	UPROPERTY(VisibleAnywhere)
		int ID; /** BuildingManager���� ������ ������ �ĺ��� */

	UPROPERTY(VisibleAnywhere)
		EBuildingState BuildingState;

	UPROPERTY(VisibleAnywhere)
		EBuildingType BuildingType;

	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConstructionTime; /** �Ǽ��ð� (s) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float HealthPoint; /** �ʱ�(����) ����� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaxHealthPoint; /** �ϼ��� ����� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float TickHealthPoint; /** 1�ʴ� �����ϴ� ����� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		FVector2D Size; /** ũ�� (NxN) */
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

	UPROPERTY(EditAnywhere, Category = "ATurret")
		int IdxOfUnderWall; /** Wall ���� �ͷ��� �Ǽ��� ��, �Ʒ��� Wall�� �ε����� �����ϴ� �뵵�� ����մϴ�. */

protected:
	virtual void InitHelthPointBar() override;
	virtual void BeginPlayHelthPointBar() final;
	virtual void TickHelthPointBar() final;

	void InitRootComp();
	virtual void InitStat();
	virtual void InitConstructBuilding();
	virtual void InitBuilding();
	void InitMaterial();

	void AddConstructBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void AddBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void AddBuildingSkMC(USkeletalMeshComponent** SkeletalMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

	UFUNCTION(Category = "Overlap")
		virtual void OnOverlapBegin_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "Overlap")
		virtual void OnOverlapEnd_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TickOfConstructable();
	void TickOfConsumeAndProduct(float DeltaTime);

public:
	FORCEINLINE void SetBuildingManager(class ABuildingManager* pBuildingManager) { this->BuildingManager = pBuildingManager; }

	UFUNCTION(Category = "Stat")
		void SetHealthPoint(float Value);
	UFUNCTION(Category = "Building")
		void SetBuildingMaterials();
	UFUNCTION(Category = "Building")
		void SetConstructableMaterial();
	UFUNCTION(Category = "Building")
		void SetUnConstructableMaterial();

	UFUNCTION(Category = "Rotation")
		void Rotating(float Value);

	UFUNCTION(Category = "ABuilding")
		bool Constructing(); /** EBuildingState::Constructable -> Constructing */
	UFUNCTION(Category = "ABuilding")
		void CheckConstructable(); /** EBuildingState::Constructable -> Constructing */
	UFUNCTION(Category = "ABuilding")
		void CompleteConstructing(); /** EBuildingState::Constructing -> Constructed */
	UFUNCTION(Category = "ABuilding")
		void Destroying(); /** EBuildingState::Destroying */

	////////////
	// ��Ʈ��ũ
	////////////
	void SetInfoOfBuilding_Spawn(class cInfoOfBuilding_Spawn& Spawn);
	class cInfoOfBuilding_Spawn GetInfoOfBuilding_Spawn();
	void SetInfoOfBuilding_Stat(class cInfoOfBuilding_Stat& Stat);
	class cInfoOfBuilding_Stat GetInfoOfBuilding_Stat();
	void SetInfoOfBuilding(class cInfoOfBuilding& InfoOfBuilding);
	class cInfoOfBuilding GetInfoOfBuilding();
};
