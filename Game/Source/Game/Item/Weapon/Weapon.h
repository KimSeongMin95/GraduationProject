// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 직접 정의한 헤더 선언 : Start ***/
#include "MyGameModeBase.h"
/*** 직접 정의한 헤더 선언 : End ***/

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Components/ArrowComponent.h"
#include "Engine.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Weapon.generated.h"

// DECLARE_DELEGATE(FFireDelegate);

UENUM()
enum class EWeaponType : uint8
{
	Pistol = 0,
	Rifle = 1,
	Launcher = 2
};

UCLASS()
class GAME_API AWeapon : public AItem
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** Item : Start ***/
public:
	virtual void InitItem() override; /** 아이템 초기화 */

	virtual void Droped() final;   /** 땅에 떨어진 상태 */
	virtual void Acquired() final; /** 획득된 상태 */
/*** Item : End ***/

/*** Stat : Start ***/
public:
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Stat")
		int LimitedLevel; /** 제한 전투력 */

	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackPower; /** 공격력 */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackSpeed; /** 공격속도 (쿨타임) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackRange; /** 사정거리 */

	UPROPERTY(EditAnywhere, Category = "Stat")
		float FireCoolTime; /** 발사 쿨타임 */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ReloadTime; /** 재장전 쿨타임 */

	UPROPERTY(EditAnywhere, Category = "Stat")
		float CurrentNumOfBullets; /** 현재 총알 개수 */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaximumNumOfBullets; /** 최대 총알 개수 */

	UPROPERTY(VisibleAnywhere, Category = "Stat")
		FName SocketName; /** Pioneer의 Skeleton Socket에 붙이기 위한 소켓명 */

protected:
	virtual void InitStat();
/*** Stat : End ***/

/*** Weapon : Start ***/
private:
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		/** weapon의 main skeletal mesh. */
		class USkeletalMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon") 
		/** Projectile이 Spawn되는 방향을 표시 */
		class UArrowComponent* ArrowComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon") 
		/** main skeletal mesh의 skeleton */
		class USkeleton* Skeleton = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		/** main skeletal mesh의 무기를 발사하는 AinmSequence */
		class UAnimSequence* FireAnimSequence = nullptr;

protected:
	void InitMesh(const TCHAR* ReferencePath);
	void InitArrowComponent(FRotator Rotatation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitSkeleton(const TCHAR* ReferencePath);
	void InitFireAnimSequence(const TCHAR* ReferencePath);

public:
	/** Returns Mesh **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return Mesh; }

	/** Returns ArrowComponent **/
	FORCEINLINE class UArrowComponent* GetArrowComponent() const { return ArrowComponent; }

	/** Returns Skeleton **/
	FORCEINLINE class USkeleton* GetSkeleton() const { return Skeleton; }

	/** Returns FireAnimSequence **/
	FORCEINLINE class UAnimSequence* GetFireAnimSequence() const { return FireAnimSequence; }



	UFUNCTION()
		virtual bool Fire(); /** 무기를 발사 */
/*** Weapon : End ***/

};
