// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.

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
		class USkeletalMeshComponent* WeaponMesh = nullptr; /** ������ mesh�Դϴ�.*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon") 
		class UArrowComponent* ArrowComponent = nullptr; /** Projectile�� Spawn�Ǵ� ������ ǥ���մϴ�.*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon") 
		class USkeleton* Skeleton = nullptr; /** WeaponMesh�� skeleton�Դϴ�.*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		class UAnimSequence* FireAnimSequence = nullptr; /** WeaponMesh�� ���⸦ �߻��ϴ� AinmSequence�Դϴ�.*/

public:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		EWeaponType WeaponType; /** ���⸦ ���ѷ�, ���ѷ�, ��ȭ����� �����մϴ�. */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		int WeaponNumbering; /** �� ������ ������ �ĺ����Դϴ�. */

	UPROPERTY(EditAnywhere, Category = "Stat")
		int LimitedLevel; /** ���� ���� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackPower; /** ���ݷ� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackSpeed; /** ���ݼӵ� (��Ÿ��) */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackRange; /** �����Ÿ� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float FireCoolTime; /** �߻� ��Ÿ�� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float ReloadTime; /** ������ ��Ÿ�� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float CurrentNumOfBullets; /** ���� �Ѿ� ���� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaximumNumOfBullets; /** �ִ� �Ѿ� ���� */

	UPROPERTY(VisibleAnywhere, Category = "Stat")
		FName SocketName; /** Pioneer�� Skeleton Socket�� ���̱� ���� ���ϸ��Դϴ�. */

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
		virtual bool Fire(const int& IDOfPioneer, const int& SocketIDOfPioneer); /** ���⸦ �߻��մϴ�. */
	UFUNCTION()
		void FireNetwork(const int& IDOfPioneer, const FTransform& Transform);

};
