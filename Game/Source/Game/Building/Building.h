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

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

/*** For TArray_2Dimension : Start ***/
USTRUCT()
struct FTArrayOfUMaterialInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
		TArray<class UMaterialInterface*> Object;
};
/*** For TArray_2Dimension : End ***/

UCLASS()
class GAME_API ABuilding : public AActor
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
		class USphereComponent* SphereComponent = nullptr;

	void InitRootComponent();
/*** RootComponent : End ***/

/*** Statements : Start ***/
public:
	UPROPERTY(EditAnywhere)
		float HP; /** 초기 생명력 */
	UPROPERTY(EditAnywhere)
		float CompleteHP; /** 완성된 생명력 */

	UPROPERTY(EditAnywhere)
		FVector2D Size; /** 크기 (NxN) */

	UPROPERTY(EditAnywhere)
		float ConstructionTime; /** 건설시간 (s) */

	UPROPERTY(EditAnywhere)
		float NeedMineral; /** 건설재료 무기물 (kg) */
	UPROPERTY(EditAnywhere)
		float NeedOrganicMatter; /** 건설재료 유기물 (kg) */

	UPROPERTY(EditAnywhere)
		float ConsumeMineral; /** 소비 무기물 (kg/h) */
	UPROPERTY(EditAnywhere)
		float ConsumeOrganicMatter; /** 소비 유기물 (kg/h) */
	UPROPERTY(EditAnywhere)
		float ConsumeElectricPower; /** 소비 전력 (MW) */

	UPROPERTY(EditAnywhere)
		float ProductionMineral; /** 생산 무기물 (kg/h) */
	UPROPERTY(EditAnywhere)
		float ProductionOrganicMatter; /** 생산 유기물 (kg/h) */
	UPROPERTY(EditAnywhere)
		float ProductionElectricPower; /** 생산 전력 (MW) */

	virtual void InitStatement();
/*** Statements : End ***/

/*** ConstructBuildingStaticMeshComponent : Start ***/
public:
	/*** StaticMesh : Start ***/
	UPROPERTY(VisibleAnywhere)
		TArray<class UStaticMeshComponent*> ConstructBuildingSMCs;
	/*** StaticMesh : End ***/

	/*** SkeltalMesh : Start ***/
	UPROPERTY(VisibleAnywhere)
		TArray<class UStaticMeshComponent*> ConstructBuildingSkMCs;
	/*** SkeltalMesh : End ***/

	UFUNCTION()
		virtual void OnOverlapBegin_ConstructBuilding(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void InitConstructBuilding();
	void AddConstructBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind = TEXT("NULL"), FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
public:
	bool bIsConstructing;
	bool bCompleted;

	/*** StaticMesh : Start ***/
	UPROPERTY(VisibleAnywhere)
		TArray<class UStaticMeshComponent*> BuildingSMCs;
	// TArray는 1차원만 사용가능하므로 구조체를 이용하여 2차원으로 사용합니다.
	UPROPERTY(VisibleAnywhere)
		TArray<FTArrayOfUMaterialInterface> BuildingSMCsMaterials; /** 기존 머터리얼들을 저장 */
	/*** StaticMesh : End ***/
	
	/*** SkeltalMesh : Start ***/
	UPROPERTY(VisibleAnywhere)
		TArray<class USkeletalMeshComponent*> BuildingSkMCs;
	// TArray는 1차원만 사용가능하므로 구조체를 이용하여 2차원으로 사용합니다.
	UPROPERTY(VisibleAnywhere)
		TArray<FTArrayOfUMaterialInterface> BuildingSkMCsMaterials; /** 기존 머터리얼들을 저장 */
	/*** SkeltalMesh : End ***/

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
		class UMaterial* ConstructableMaterial = nullptr;
	void SetConstructableMaterial();

	UPROPERTY(VisibleAnywhere)
		class UMaterial* UnConstructableMaterial = nullptr;
	void SetUnConstructableMaterial();

	void InitMaterial();
/*** Material : End ***/

/*** Rotation : Start ***/
public:
	//void Locating(FVector position);
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
