// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// 기본 생성 헤더
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pioneer.generated.h"

// 추가한 헤더


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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//public: // Variables
//	// 캐릭터 공중에서 카메라의 위치를 조정합니다.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
//		class USpringArmComponent* CameraBoom; // 카메라 제어에 사용됩니다.
//
//	// 따라다니는 카메라
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
//		class UCameraComponent* FollowCamera; // 게임 화면을 보여주고 플레이어를 따라다니는 카메라입니다.
//
//	// 스프라이트는 HUD나 텍스처보다 제어하기 쉽고 더 멋진 효과를 그리는 데 사용됩니다.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
//		class UPaperSpriteComponent* EffectSprite; // 필요할 때마다 화면에 그리도록 사용할 수 있습니다.
//
//	UCapsuleComponent* CapsuleComp;
//	
//
//	// 기본 선회율로 초당 회전 각도로 사용됩니다. 값에 따라 최종 선회율에 영향을 미칩니다.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
//		float BaseTurnRate; // 카메라 회전 비율을 제어하기 위한 각도
//
//	// 기본 시선 상->하향 비율로 초당 각도로 사용됩니다. 값에 따라 최종 비율에 영향을 미칩니다.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
//		float BaseLookUpRate; // 시선 상향을 위해 사용합니다.
//
//	// 기본 점프 속도
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Attributes")
//		float JumppingVelocity;
//
//	// 개척민이 살아 있는지 아닌지 여부
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Attributes")
//		bool IsAlive;
//
//public:	// Functions 
//	// 입력 활성화 또는 비활성화
//	UFUNCTION(BlueprintCallable, Category = "Player Attributes")
//		void OnSetPlayerController(bool status);
//
//	// CameraBoom 하위 객체를 반환합니다.
//	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
//
//	// FollowCamera 하위 객체를 반환합니다.
//	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
//
//	// 따라다니는 카메라르 회전시키는 함수입니다.
//	void TurnAtRate(float Rate);
//
//	// 따라다니는 카메라에 시선 상향 비율을 적용하기 위한 함수입니다.
//	void LookUpAtRate(float Rate);
};
