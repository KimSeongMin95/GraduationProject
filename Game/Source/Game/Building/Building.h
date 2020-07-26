// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
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
	Constructable = 0,	/** 건설할 수 있는지 확인하는 상태 */
	Constructing = 1,	/** 건설중인 상태 */
	Constructed = 2,	/** 건설이 완료된 상태*/
	Destroying = 3		/** 생명력이 0이하가 되어 소멸되는 상태 */
};

USTRUCT()
struct FTArrayOfUMaterialInterface /** TArray는 1차원만 사용가능하므로 구조체를 이용하여 2차원으로 사용합니다.*/
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
		TArray<class AActor*> OverlappedActors; /** 충돌한 액터들을 모두 저장하고 벗어나면 제거합니다. */

	UPROPERTY(VisibleAnywhere, Category = "Building")
		TArray<FTArrayOfUMaterialInterface> BuildingSMCsMaterials; /** StaticMeshs의 원본 머터리얼들을 저장합니다. */
	UPROPERTY(VisibleAnywhere, Category = "Building")
		TArray<FTArrayOfUMaterialInterface> BuildingSkMCsMaterials; /** SkeltalMeshs의 원본 머터리얼들을 저장합니다. */

	UPROPERTY(VisibleAnywhere)
		class UMaterial* ConstructableMaterial = nullptr; /** EBuildingState::Constructable일 때 사용할 연두색 반투명한 머터리얼 */
	UPROPERTY(VisibleAnywhere)
		class UMaterial* UnConstructableMaterial = nullptr; /** EBuildingState::Constructable일 때 사용할 빨간색 반투명한 머터리얼 */
	
	FTimerHandle TimerOfConstructing;

public:
	UPROPERTY(VisibleAnywhere, Category = "BuildingManager")
		class ABuildingManager* BuildingManager = nullptr;

	UPROPERTY(VisibleAnywhere)
		bool bDying;

	UPROPERTY(VisibleAnywhere)
		int ID; /** BuildingManager에서 관리할 고유한 식별자 */

	UPROPERTY(VisibleAnywhere)
		EBuildingState BuildingState;

	UPROPERTY(VisibleAnywhere)
		EBuildingType BuildingType;

	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConstructionTime; /** 건설시간 (s) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float HealthPoint; /** 초기(현재) 생명력 */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaxHealthPoint; /** 완성된 생명력 */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float TickHealthPoint; /** 1초당 증가하는 생명력 */
	UPROPERTY(EditAnywhere, Category = "Stat")
		FVector2D Size; /** 크기 (NxN) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float NeedMineral; /** 건설재료 무기물 (kg) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float NeedOrganicMatter; /** 건설재료 유기물 (kg) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConsumeMineral; /** 소비 무기물 (kg/h) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConsumeOrganicMatter; /** 소비 유기물 (kg/h) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConsumeElectricPower; /** 소비 전력 (MW) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ProductionMineral; /** 생산 무기물 (kg/h) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ProductionOrganicMatter; /** 생산 유기물 (kg/h) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ProductionElectricPower; /** 생산 전력 (MW) */

	UPROPERTY(EditAnywhere, Category = "ATurret")
		int IdxOfUnderWall; /** Wall 위에 터렛을 건설할 때, 아래의 Wall의 인덱스를 저장하는 용도로 사용합니다. */

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
	// 네트워크
	////////////
	void SetInfoOfBuilding_Spawn(class cInfoOfBuilding_Spawn& Spawn);
	class cInfoOfBuilding_Spawn GetInfoOfBuilding_Spawn();
	void SetInfoOfBuilding_Stat(class cInfoOfBuilding_Stat& Stat);
	class cInfoOfBuilding_Stat GetInfoOfBuilding_Stat();
	void SetInfoOfBuilding(class cInfoOfBuilding& InfoOfBuilding);
	class cInfoOfBuilding GetInfoOfBuilding();
};
