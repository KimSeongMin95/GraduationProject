// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "PioneerAnimInstance.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
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

//#include "Animation/AnimMontage.h" // �ӽ�
//#include "Animation/AnimInstance.h" // �ӽ�

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	//// Called to bind functionality to input
	//// ���ε��� Ű �Է��� �����մϴ�.
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // APawn �������̽�     

private:
	/*** Components : Start ***/
	//UPROPERTY(EditAnywhere)
	//	class UStaticMeshComponent* StaticMeshComponent; /** �ӽ÷� StaticMesh�� �����մϴ�. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom; /** ĳ���� ������ ī�޶��� ��ġ�� �����մϴ�. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent; /** ����ٴϴ� ī�޶��Դϴ�. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld; /** A decal that projects to the cursor location. */

public: // Animation �뵵
	// ??? CDO constructor���� ������Ű�� �༮�� Blueprint���� ������ �� ����� �Ѵ�???
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

	void SetCursorToWorld(); /** CursorToWorld�� ������ǥ�� ����ȸ���� �����մϴ�.*/

	//// �⺻ ��ȸ���� �ʴ� ȸ�� ������ ���˴ϴ�. ���� ���� ���� ��ȸ���� ������ ��Ĩ�ϴ�.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	//	float BaseTurnRate;

	//// �⺻ �ü� ��-���� ������ �ʴ� ������ ���˴ϴ�. ���� ���� ���� ������ ������ ��Ĩ�ϴ�.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	//	float BaseLookUpRate;

	//// �ƽ��̳� �޴�, �÷��̾ �׾��� ���� ���� ��Ȳ���� �÷��̾ ��Ȱ��ȭ�մϴ�.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerAttributes")
	//	bool IsControlable;

	//// �Է� Ȱ��ȭ �Ǵ� ��Ȱ��ȭ�մϴ�.
	//UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	//	void OnSetPlayerController(bool status);


protected:
	//// �÷��̾ �յڷ� �̵���Ű�� �Լ��Դϴ�.
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void MoveForward(float value);

	//// �÷��̾ �¿�� �̵���Ű�� �Լ��Դϴ�.
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void MoveRight(float value);

	//// �÷��̾� ȸ�� ������
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void EvasionRoll();

	//// �÷��̾� ȸ�� ������ ����
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void StopEvasionRoll();

	//// FollowCamera�� ȸ����Ű�� ���� �Լ��Դϴ�
	//void TurnAtRate(float rate);

	//// FollowCamera�� �ü� ���� ������ �����ϱ� ���� �Լ��Դϴ�.
	//void LookUpAtRate(float rate);

public:
	/*** ī�޶� ������ PIE�� �����մϴ�. : Start ***/
	UPROPERTY(EditAnywhere) FVector CameraBoomLocation; // ArmSpring�� World ��ǥ�Դϴ�.
	UPROPERTY(EditAnywhere) FRotator CameraBoomRotation; // ArmSpring�� World ȸ���Դϴ�.
	UPROPERTY(EditAnywhere) float TargetArmLength; // ArmSpring�� FollowCamera���� �Ÿ��Դϴ�.
	UPROPERTY(EditAnywhere) int CameraLagSpeed; // �ε巯�� ī�޶� ��ȯ �ӵ��Դϴ�.
	void SetCameraBoomSettings(); // Tick()���� ȣ���մϴ�.
	/*** ī�޶� ������ PIE�� �����մϴ�. : End ***/
};