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
		float HP; /** �ʱ� ����� */
	UPROPERTY(EditAnywhere)
		float CompleteHP; /** �ϼ��� ����� */

	UPROPERTY(EditAnywhere)
		FVector2D Size; /** ũ�� (NxN) */

	UPROPERTY(EditAnywhere)
		float ConstructionTime; /** �Ǽ��ð� (s) */

	UPROPERTY(EditAnywhere)
		float NeedMineral; /** �Ǽ���� ���⹰ (kg) */
	UPROPERTY(EditAnywhere)
		float NeedOrganicMatter; /** �Ǽ���� ���⹰ (kg) */

	UPROPERTY(EditAnywhere)
		float ConsumeMineral; /** �Һ� ���⹰ (kg/h) */
	UPROPERTY(EditAnywhere)
		float ConsumeOrganicMatter; /** �Һ� ���⹰ (kg/h) */
	UPROPERTY(EditAnywhere)
		float ConsumeElectricPower; /** �Һ� ���� (MW) */

	UPROPERTY(EditAnywhere)
		float ProductionMineral; /** ���� ���⹰ (kg/h) */
	UPROPERTY(EditAnywhere)
		float ProductionOrganicMatter; /** ���� ���⹰ (kg/h) */
	UPROPERTY(EditAnywhere)
		float ProductionElectricPower; /** ���� ���� (MW) */

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
	// TArray�� 1������ ��밡���ϹǷ� ����ü�� �̿��Ͽ� 2�������� ����մϴ�.
	UPROPERTY(VisibleAnywhere)
		TArray<FTArrayOfUMaterialInterface> BuildingSMCsMaterials; /** ���� ���͸������ ���� */
	/*** StaticMesh : End ***/
	
	/*** SkeltalMesh : Start ***/
	UPROPERTY(VisibleAnywhere)
		TArray<class USkeletalMeshComponent*> BuildingSkMCs;
	// TArray�� 1������ ��밡���ϹǷ� ����ü�� �̿��Ͽ� 2�������� ����մϴ�.
	UPROPERTY(VisibleAnywhere)
		TArray<FTArrayOfUMaterialInterface> BuildingSkMCsMaterials; /** ���� ���͸������ ���� */
	/*** SkeltalMesh : End ***/

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
