// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"

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

	// Called to bind functionality to input
	// 바인딩한 키 입력을 매핑합니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // APawn 인터페이스     

	// Camera boom positioning the camera behind the character
	// 캐릭터 뒤편에서 카메라의 위치를 조정합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	// Follow camera
	// 따라다니는 카메라입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	// 기본 선회율로 초당 회전 각도로 사용됩니다. 값에 따라 최종 선회율에 영향을 미칩니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	// 기본 시성 상-하향 비율로 초당 각도로 사용됩니다. 값에 따라 최종 비율에 영향을 미칩니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	// 컷신이나 메뉴, 플레이어가 죽었을 떄와 같은 상황에서 플레이어를 비활성화합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerAttributes")
		bool IsControlable;

	// 입력 활성화 또는 비활성화합니다.
	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
		void OnSetPlayerController(bool status);

	// CameraBoom 하위 객체를 반환합니다.
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCamera 하위 객체를 반환합니다.
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// 플레이어를 앞뒤로 이동시키는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveForward(float value);

	// 플레이어를 좌우로 이동시키는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveRight(float value);

	// 플레이어 회피 구르기
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void EvasionRoll();

	// 플레이어 회피 구르기 멈춤
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void StopEvasionRoll();

	// FollowCamera를 회전시키기 위한 함수입니다
	void TurnAtRate(float rate);

	// FollowCamera에 시선 상향 비율을 적용하기 위한 함수입니다.
	void LookUpAtRate(float rate);


};