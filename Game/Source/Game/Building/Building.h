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
enum class EBuildingType : uint8
{
	Floor,

	Wall,
	Stairs,
	Gate,
	InorganicMine,
	OrganicMine,
	NuclearFusionPowerPlant,
	AssaultRifleTurret,
	SniperRifleTurret,
	RocketLauncherTurret,

	ResearchInstitute,
	WeaponFactory
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
/** TArray�� 1������ ��밡���ϹǷ� ����ü�� �̿��Ͽ� 2�������� ����մϴ�.*/
struct FTArrayOfUMaterialInterface 
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
	ABuilding();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() override;
	virtual void BeginPlayHelthPointBar() final;
	virtual void TickHelthPointBar() final;
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "ABuilding")
		/** RootComponent */
		class USphereComponent* SphereComponent = nullptr; 

protected:
	UPROPERTY(VisibleAnywhere, Category = "ConstructBuilding")
		/** StaticMeshs */
		TArray<class UStaticMeshComponent*> ConstructBuildingSMCs;

	UPROPERTY(VisibleAnywhere, Category = "ConstructBuilding")
		/** SkeltalMeshs */
		TArray<class UStaticMeshComponent*> ConstructBuildingSkMCs;


	UPROPERTY(VisibleAnywhere)
		/** �浹�� ���͵��� ��� �����ϰ� ����� ���� */
		TArray<class AActor*> OverlappedActors;


	UPROPERTY(VisibleAnywhere, Category = "Building")
		/** StaticMeshs */
		TArray<class UStaticMeshComponent*> BuildingSMCs;

	UPROPERTY(VisibleAnywhere, Category = "Building")
		/** SkeltalMeshs */
		TArray<class USkeletalMeshComponent*> BuildingSkMCs;

	UPROPERTY(VisibleAnywhere, Category = "Building")
		/** StaticMeshs�� ���� ���͸������ �����մϴ�.
		TArray�� 1������ ��밡���ϹǷ� ����ü�� �̿��Ͽ� 2�������� ����մϴ�.*/
		TArray<FTArrayOfUMaterialInterface> BuildingSMCsMaterials;

	UPROPERTY(VisibleAnywhere, Category = "Building")
		/** SkeltalMeshs�� ���� ���͸������ �����մϴ�.
		TArray�� 1������ ��밡���ϹǷ� ����ü�� �̿��Ͽ� 2�������� ����մϴ�.*/
		TArray<FTArrayOfUMaterialInterface> BuildingSkMCsMaterials;


	UPROPERTY(VisibleAnywhere)
		/** EBuildingState::Constructable�� �� ����� ���λ� �������� ���͸��� */
		class UMaterial* ConstructableMaterial = nullptr;

	UPROPERTY(VisibleAnywhere)
		/** EBuildingState::Constructable�� �� ����� ������ �������� ���͸��� */
		class UMaterial* UnConstructableMaterial = nullptr;

	UPROPERTY(EditAnywhere)
		float TimerOfTickOfConsumeAndProduct;

	class cServerSocketInGame* ServerSocketInGame = nullptr;
	class cClientSocketInGame* ClientSocketInGame = nullptr;
	

public:
	UPROPERTY(VisibleAnywhere)
		/** �浹�� ���͵��� ��� �����ϰ� ����� ���� */
		bool bDying;

	UPROPERTY(VisibleAnywhere, Category = "BuildingManager")
		class ABuildingManager* BuildingManager = nullptr;

	UPROPERTY(VisibleAnywhere)
		/** BuildingManager���� ������ ������ �ĺ��� */
		int ID;

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
		/** Wall ���� �ͷ��� �Ǽ��� �� �Ʒ��� Wall�� �ε����� �����ϴ� �뵵 */
		int IdxOfUnderWall;


private:


protected:
	virtual void InitStat();
	virtual void InitConstructBuilding();
	virtual void InitBuilding();
	void InitMaterial();

	void AddConstructBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	
	void AddBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	
	/** SubStaticMeshComp�� ���� AddBuildingSMC(SubStaticMeshComp) �ϰ� �����;� ��. */
	void AddBuildingSkMC(USkeletalMeshComponent** SkeletalMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);


	UFUNCTION(Category = "Building")
		virtual void OnOverlapBegin_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(Category = "Building")
		virtual void OnOverlapEnd_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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
		/** EBuildingState::Constructable -> Constructing */
		bool Constructing();

	UFUNCTION(Category = "Timer")
		/** EBuildingState::Constructable -> Constructing */
		void CheckConstructable();
	FTimerHandle TimerOfConstructing;

	UFUNCTION(Category = "ABuilding")
		/** EBuildingState::Constructing -> Constructed */
		void CompleteConstructing();

	UFUNCTION(Category = "ABuilding")
		/** EBuildingState::Destroying */
		void Destroying();


	///////////
	// ��Ʈ��ũ
	///////////
	void SetInfoOfBuilding_Spawn(class cInfoOfBuilding_Spawn& Spawn);
	class cInfoOfBuilding_Spawn GetInfoOfBuilding_Spawn();

	void SetInfoOfBuilding_Stat(class cInfoOfBuilding_Stat& Stat);
	class cInfoOfBuilding_Stat GetInfoOfBuilding_Stat();

	void SetInfoOfBuilding(class cInfoOfBuilding& InfoOfBuilding);
	class cInfoOfBuilding GetInfoOfBuilding();
/*** ABuilding : End ***/
};
