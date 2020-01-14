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
	virtual void InitItem() override; /** 초기화 */

	virtual void Droped() final;   /** 땅에 떨어진 상태 */
	virtual void Acquired() final; /** 획득된 상태 */
	/*** Item : End ***/

public:
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere)
		class UArrowComponent* ProjectileSpawnPoint = nullptr;

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

	virtual void InitStat();
/*** Stat : End ***/

public:
	UPROPERTY(EditAnywhere)
		class USkeleton* Skeleton;

	UPROPERTY(EditAnywhere)
		class UAnimSequence* AnimSequence;

	UFUNCTION()
		virtual bool Fire();

/*** 블루프린트의 Event : Start ***/
public:
	//// BlueprintImplementableEvent: 헤더에만 선언해줘서 블루프린트에서 정의하는 매크로
	//UFUNCTION(BlueprintImplementableEvent)
	//	void PullTrigger();

	// BlueprintNaitveEvent: C++에서 정의하는 이벤트
	/*UFUNCTION(BlueprintNaitveEvent)
	void PullTrigger();*/
/*** 블루프린트의 Event : End ***/

	// FFireDelegate FireDelegate;

};
