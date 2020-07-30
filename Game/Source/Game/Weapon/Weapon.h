// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM()
enum class EWeaponType : uint8
{
	Pistol = 0,
	Rifle = 1,
	Launcher = 2
};

UCLASS()
class GAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual ~AWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		class USkeletalMeshComponent* WeaponMesh = nullptr; /** 무기의 mesh입니다.*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon") 
		class UArrowComponent* ArrowComponent = nullptr; /** Projectile이 Spawn되는 방향을 표시합니다.*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon") 
		class USkeleton* Skeleton = nullptr; /** WeaponMesh의 skeleton입니다.*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		class UAnimSequence* FireAnimSequence = nullptr; /** WeaponMesh의 무기를 발사하는 AinmSequence입니다.*/

public:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		EWeaponType WeaponType; /** 무기를 권총류, 소총류, 중화기류로 구분합니다. */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		int WeaponNumbering; /** 각 무기의 지정된 식별자입니다. */

	UPROPERTY(EditAnywhere, Category = "Stat")
		int LimitedLevel; /** 제한 레벨 */
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
		FName SocketName; /** Pioneer의 Skeleton Socket에 붙이기 위한 소켓명입니다. */

protected:
	virtual void InitStat();
	virtual void InitWeapon();
	void InitWeaponMesh(const TCHAR* ReferencePath);
	void InitArrowComponent(const FRotator& Rotatation = FRotator::ZeroRotator, const FVector& Location = FVector::ZeroVector);
	void InitSkeleton(const TCHAR* ReferencePath);
	void InitFireAnimSequence(const TCHAR* ReferencePath);

public:
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return WeaponMesh; }
	FORCEINLINE class UArrowComponent* GetArrowComponent() const { return ArrowComponent; }
	FORCEINLINE class USkeleton* GetSkeleton() const { return Skeleton; }
	FORCEINLINE class UAnimSequence* GetFireAnimSequence() const { return FireAnimSequence; }

	UFUNCTION()
		virtual bool Fire(const int& IDOfPioneer, const int& SocketIDOfPioneer); /** 무기를 발사합니다. */
	UFUNCTION()
		void FireNetwork(const int& IDOfPioneer, const FTransform& Transform);

};
