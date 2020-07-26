// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PioneerController.generated.h"

UCLASS()
class GAME_API APioneerController : public APlayerController
{
	GENERATED_BODY()

public:
	APioneerController();
	virtual ~APioneerController();

protected:
	/** �Ϲ� Tick() �Լ��� ��𼭳� �۵��ϴ� �ݸ鿡, PlayerTick() �Լ��� Player Controller�� PlayerInput ��ü�� �ִ� ��쿡�� ȣ��˴ϴ�.
	���� ���÷� ����Ǵ� Player Controller������ �÷��̾� ƽ�� ȣ��˴ϴ�.
	���� ��Ƽ�÷��� �����̶��, �ڱ� �ڽ��� �÷��̾� ��Ʈ�ѷ������� �÷��̾� ƽ�� ȣ��˴ϴ�. */
	virtual void PlayerTick(float DeltaTime) final;
	virtual void SetupInputComponent() final;

private:
	UPROPERTY(VisibleAnywhere, Category = "PioneerController")
		class APioneer* Pioneer = nullptr; /** �����ϴ� APioneer�� �����մϴ�. */

	UPROPERTY(VisibleAnywhere, Category = "PioneerController")
		class APioneerManager* PioneerManager = nullptr;

	bool bScoreBoard;

	bool bObservation; /** �������� */

	float PlayTickDeltaTime;

public:
	uint32 bMoveToMouseCursor : 1;  /** true�� ���콺 Ŀ���� navigating �մϴ�. */

public:
	virtual void SetViewTargetWithBlend(class AActor* NewViewTarget, float BlendTime = 0.0f, enum EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true) final; // bLockOutgoing�� true�� �ؾ� �߰��� ���ٰ� ViewTarget�� ����Ǿ �ڿ������� �Ѿ�ϴ�.

	virtual void OnPossess(APawn* InPawn) final;
	virtual void OnUnPossess() final;

	void MoveToMouseCursor();

	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void MoveForward(float Value); /** �÷��̾ �յڷ� �̵���Ű�� �Լ��Դϴ�. */
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void MoveRight(float Value); /** �÷��̾ �¿�� �̵���Ű�� �Լ��Դϴ�. */

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ZoomInOrZoomOut(float Value);

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void FireWeapon(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ChangePreviousWeapon();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ChangeNextWeapon();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ArmOrDisArmWeapon();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ConstructingMode();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ESC_ConstructingMode();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void SpawnBuilding(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void RotatingBuilding(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void PlaceBuilding();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void Menu();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ScoreBoard(float Value);

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ObservingLeft();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ObservingRight();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ObservingFree();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ObservingPossess();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void FreeViewPoint_MoveForward(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void FreeViewPoint_MoveRight(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void FreeViewPoint_MoveUp(float Value);

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void EasterEgg(float Value);

	void SetPioneerManager(class APioneerManager* PioneerManager);
};
