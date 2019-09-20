// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// �⺻ ���� ���
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pioneer.generated.h"

// �߰��� ���


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
	// ���ε��� Ű �Է��� �����մϴ�.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//public: // Variables
//	// ĳ���� ���߿��� ī�޶��� ��ġ�� �����մϴ�.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
//		class USpringArmComponent* CameraBoom; // ī�޶� ��� ���˴ϴ�.
//
//	// ����ٴϴ� ī�޶�
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
//		class UCameraComponent* FollowCamera; // ���� ȭ���� �����ְ� �÷��̾ ����ٴϴ� ī�޶��Դϴ�.
//
//	// ��������Ʈ�� HUD�� �ؽ�ó���� �����ϱ� ���� �� ���� ȿ���� �׸��� �� ���˴ϴ�.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
//		class UPaperSpriteComponent* EffectSprite; // �ʿ��� ������ ȭ�鿡 �׸����� ����� �� �ֽ��ϴ�.
//
//	UCapsuleComponent* CapsuleComp;
//	
//
//	// �⺻ ��ȸ���� �ʴ� ȸ�� ������ ���˴ϴ�. ���� ���� ���� ��ȸ���� ������ ��Ĩ�ϴ�.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
//		float BaseTurnRate; // ī�޶� ȸ�� ������ �����ϱ� ���� ����
//
//	// �⺻ �ü� ��->���� ������ �ʴ� ������ ���˴ϴ�. ���� ���� ���� ������ ������ ��Ĩ�ϴ�.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
//		float BaseLookUpRate; // �ü� ������ ���� ����մϴ�.
//
//	// �⺻ ���� �ӵ�
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Attributes")
//		float JumppingVelocity;
//
//	// ��ô���� ��� �ִ��� �ƴ��� ����
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Attributes")
//		bool IsAlive;
//
//public:	// Functions 
//	// �Է� Ȱ��ȭ �Ǵ� ��Ȱ��ȭ
//	UFUNCTION(BlueprintCallable, Category = "Player Attributes")
//		void OnSetPlayerController(bool status);
//
//	// CameraBoom ���� ��ü�� ��ȯ�մϴ�.
//	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
//
//	// FollowCamera ���� ��ü�� ��ȯ�մϴ�.
//	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
//
//	// ����ٴϴ� ī�޶� ȸ����Ű�� �Լ��Դϴ�.
//	void TurnAtRate(float Rate);
//
//	// ����ٴϴ� ī�޶� �ü� ���� ������ �����ϱ� ���� �Լ��Դϴ�.
//	void LookUpAtRate(float Rate);
};
