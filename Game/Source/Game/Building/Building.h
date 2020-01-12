// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Engine/TriggerVolume.h" // For ATriggerVolume::StaticClass()
/*** 언리얼엔진 헤더 선언 : End ***/

/*** Interface 헤더 선언 : Start ***/
#include "Interface/HealthPointBarInterface.h"
/*** Interface 헤더 선언 : Start ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

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
		float HealthPoint; /** 초기 생명력 */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaxHealthPoint; /** 완성된 생명력 */

	UPROPERTY(EditAnywhere, Category = "Stat")
		FVector2D Size; /** 크기 (NxN) */

	UPROPERTY(EditAnywhere, Category = "Stat")
		float ConstructionTime; /** 건설시간 (s) */

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
	// TArray는 1차원만 사용가능하므로 구조체를 이용하여 2차원으로 사용합니다.
	UPROPERTY(VisibleAnywhere)
		TArray<FTArrayOfUMaterialInterface> BuildingSMCsMaterials; /** 기존 머터리얼들을 저장 */
	
	// SkeltalMeshs
	UPROPERTY(VisibleAnywhere)
		TArray<class USkeletalMeshComponent*> BuildingSkMCs;
	// TArray는 1차원만 사용가능하므로 구조체를 이용하여 2차원으로 사용합니다.
	UPROPERTY(VisibleAnywhere)
		TArray<FTArrayOfUMaterialInterface> BuildingSkMCsMaterials; /** 기존 머터리얼들을 저장 */

	TArray<class AActor*> OverapedActors; /** 충돌한 액터들을 모두 저장하고 벗어나면 삭제 */
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
		class UMaterial* ConstructableMaterial = nullptr; /** 연두색 반투명한 머터리얼 */
	void SetConstructableMaterial();

	UPROPERTY(VisibleAnywhere)
		class UMaterial* UnConstructableMaterial = nullptr; /** 빨간색 반투명한 머터리얼 */
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
