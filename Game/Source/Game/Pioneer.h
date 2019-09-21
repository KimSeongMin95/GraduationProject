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

#include "Pioneer.generated.h" // �׻� �������̾�� �ϴ� ���

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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // APawn �������̽�     

	// Camera boom positioning the camera behind the character
	// ĳ���� ������ ī�޶��� ��ġ�� �����մϴ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	// Follow camera
	// ����ٴϴ� ī�޶��Դϴ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	// �⺻ ��ȸ���� �ʴ� ȸ�� ������ ���˴ϴ�. ���� ���� ���� ��ȸ���� ������ ��Ĩ�ϴ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	// �⺻ �ü� ��-���� ������ �ʴ� ������ ���˴ϴ�. ���� ���� ���� ������ ������ ��Ĩ�ϴ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	// �ƽ��̳� �޴�, �÷��̾ �׾��� ���� ���� ��Ȳ���� �÷��̾ ��Ȱ��ȭ�մϴ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerAttributes")
		bool IsControlable;

	// �Է� Ȱ��ȭ �Ǵ� ��Ȱ��ȭ�մϴ�.
	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
		void OnSetPlayerController(bool status);

	// CameraBoom ���� ��ü�� ��ȯ�մϴ�.
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCamera ���� ��ü�� ��ȯ�մϴ�.
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// �÷��̾ �յڷ� �̵���Ű�� �Լ��Դϴ�.
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveForward(float value);

	// �÷��̾ �¿�� �̵���Ű�� �Լ��Դϴ�.
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveRight(float value);

	// �÷��̾� ȸ�� ������
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void EvasionRoll();

	// �÷��̾� ȸ�� ������ ����
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void StopEvasionRoll();

	// FollowCamera�� ȸ����Ű�� ���� �Լ��Դϴ�
	void TurnAtRate(float rate);

	// FollowCamera�� �ü� ���� ������ �����ϱ� ���� �Լ��Դϴ�.
	void LookUpAtRate(float rate);


};