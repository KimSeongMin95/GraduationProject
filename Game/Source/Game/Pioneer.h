// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "PioneerAnimInstance.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
//#include "HeadMountedDisplayFunctionLibrary.h" // VR
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"

//#include "Animation/AnimMontage.h" // 임시
//#include "Animation/AnimInstance.h" // 임시

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pioneer.generated.h" // 항상 마지막이어야 하는 헤더

UCLASS()
class GAME_API APioneer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APioneer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//// Called to bind functionality to input
	//// 바인딩한 키 입력을 매핑합니다.
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // APawn 인터페이스     

private:
	/*** Components : Start ***/
	//UPROPERTY(EditAnywhere)
	//	class UStaticMeshComponent* StaticMeshComponent; /** 임시로 StaticMesh를 설정합니다. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom; /** 캐릭터 뒤편에서 카메라의 위치를 조정합니다. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent; /** 따라다니는 카메라입니다. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld; /** A decal that projects to the cursor location. */

public: // Animation 용도
	// ??? CDO constructor에서 생성시키는 녀석은 Blueprint에서 수정할 수 없어야 한다???
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		class USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere)
		class USkeleton* Skeleton;

	UPROPERTY(EditAnywhere)
		class UAnimSequence* AnimSequence;
	/*** Components : End ***/

public:
	/*** Return Components : Start ***/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TopDownCameraComponent; }

	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	/*** Return Components : End ***/

	void SetCursorToWorld(); /** CursorToWorld의 월드좌표와 월드회전을 설정합니다.*/

	//// 기본 선회율로 초당 회전 각도로 사용됩니다. 값에 따라 최종 선회율에 영향을 미칩니다.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	//	float BaseTurnRate;

	//// 기본 시성 상-하향 비율로 초당 각도로 사용됩니다. 값에 따라 최종 비율에 영향을 미칩니다.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	//	float BaseLookUpRate;

	//// 컷신이나 메뉴, 플레이어가 죽었을 떄와 같은 상황에서 플레이어를 비활성화합니다.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerAttributes")
	//	bool IsControlable;

	//// 입력 활성화 또는 비활성화합니다.
	//UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	//	void OnSetPlayerController(bool status);


protected:
	//// 플레이어를 앞뒤로 이동시키는 함수입니다.
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void MoveForward(float value);

	//// 플레이어를 좌우로 이동시키는 함수입니다.
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void MoveRight(float value);

	//// 플레이어 회피 구르기
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void EvasionRoll();

	//// 플레이어 회피 구르기 멈춤
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void StopEvasionRoll();

	//// FollowCamera를 회전시키기 위한 함수입니다
	//void TurnAtRate(float rate);

	//// FollowCamera에 시선 상향 비율을 적용하기 위한 함수입니다.
	//void LookUpAtRate(float rate);

public:
	/*** 카메라 설정을 PIE때 변경합니다. : Start ***/
	UPROPERTY(EditAnywhere) FVector CameraBoomLocation; // ArmSpring의 World 좌표입니다.
	UPROPERTY(EditAnywhere) FRotator CameraBoomRotation; // ArmSpring의 World 회전입니다.
	UPROPERTY(EditAnywhere) float TargetArmLength; // ArmSpring과 FollowCamera간의 거리입니다.
	UPROPERTY(EditAnywhere) int CameraLagSpeed; // 부드러운 카메라 전환 속도입니다.
	void SetCameraBoomSettings(); // Tick()에서 호출합니다.
	/*** 카메라 설정을 PIE때 변경합니다. : End ***/
};