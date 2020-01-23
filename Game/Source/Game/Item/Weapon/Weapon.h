// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** ���� ������ ��� ���� : Start ***/
#include "MyGameModeBase.h"
/*** ���� ������ ��� ���� : End ***/

/*** �𸮾��� ��� ���� : Start ***/
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
#include "Components/ArrowComponent.h"
#include "Engine.h"
/*** �𸮾��� ��� ���� : End ***/

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
	AWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AItem : Start ***/
protected:
	virtual void InitItem() override; /** ������ �ʱ�ȭ */

public:
	virtual void Droped() final;   /** ���� ������ ���� */
	virtual void Acquired() final; /** ȹ��� ���� */
/*** AItem : End ***/


/*** AWeapon : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		/** main skeletal mesh. */
		class USkeletalMeshComponent* WeaponMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon") 
		/** Projectile�� Spawn�Ǵ� ������ ǥ�� */
		class UArrowComponent* ArrowComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon") 
		/** main skeletal mesh�� skeleton */
		class USkeleton* Skeleton = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		/** main skeletal mesh�� ���⸦ �߻��ϴ� AinmSequence */
		class UAnimSequence* FireAnimSequence = nullptr;

public:
	EWeaponType WeaponType;


	UPROPERTY(EditAnywhere, Category = "Stat")
		int LimitedLevel; /** ���� ������ */

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
		FName SocketName; /** Pioneer�� Skeleton Socket�� ���̱� ���� ���ϸ� */


protected:
	virtual void InitStat();
	virtual void InitWeapon();
	void InitWeaponMesh(const TCHAR* ReferencePath);
	void InitArrowComponent(FRotator Rotatation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitSkeleton(const TCHAR* ReferencePath);
	void InitFireAnimSequence(const TCHAR* ReferencePath);

public:
	/** Returns Mesh **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return WeaponMesh; }

	/** Returns ArrowComponent **/
	FORCEINLINE class UArrowComponent* GetArrowComponent() const { return ArrowComponent; }

	/** Returns Skeleton **/
	FORCEINLINE class USkeleton* GetSkeleton() const { return Skeleton; }

	/** Returns FireAnimSequence **/
	FORCEINLINE class UAnimSequence* GetFireAnimSequence() const { return FireAnimSequence; }


	UFUNCTION()
		virtual bool Fire(); /** ���⸦ �߻� */
/*** AWeapon : End ***/

};
