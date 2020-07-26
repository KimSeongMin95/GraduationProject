// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerController.h"

#include "PathFinding.h"
#include "Character/Pioneer.h"
#include "Weapon/Weapon.h"
#include "GameMode/InGameMode.h"
#include "PioneerManager.h"
#include "Etc/WorldViewCameraActor.h"
#include "Building/Building.h"

APioneerController::APioneerController()
{
	Pioneer = nullptr;

	bScoreBoard = false;

	PioneerManager = nullptr;

	bObservation = true;

	bShowMouseCursor = true; // ���콺�� ���̰� �մϴ�.
	//DefaultMouseCursor = EMouseCursor::Default; // EMouseCursor::�� ���� ���콺 Ŀ�� ����� ������ �� �ֽ��ϴ�.
	DefaultMouseCursor = EMouseCursor::Crosshairs; // EMouseCursor::�� ���� ���콺 Ŀ�� ����� ������ �� �ֽ��ϴ�.

	PlayTickDeltaTime = 0.0f;
}
APioneerController::~APioneerController()
{

}

void APioneerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	PlayTickDeltaTime = DeltaTime;

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}
void APioneerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// support keyboard
	InputComponent->BindAction("SetDestination", IE_Pressed, this, &APioneerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &APioneerController::OnSetDestinationReleased);
	
	// W SŰ: �������̵�
	InputComponent->BindAxis("MoveForward", this, &APioneerController::MoveForward);
	// A DŰ: �¿����̵�
	InputComponent->BindAxis("MoveRight", this, &APioneerController::MoveRight);

	// ���콺 �� ���Ʒ�: ī�޶� ����&�ܾƿ�
	InputComponent->BindAxis("ZoomInOut", this, &APioneerController::ZoomInOrZoomOut);

	// ���콺 ��Ŭ��: ���� �߻�
	InputComponent->BindAxis("FireWeapon", this, &APioneerController::FireWeapon);
	// QŰ: ���� ����� ����
	InputComponent->BindAction("ChangePreviousWeapon", IE_Pressed, this, &APioneerController::ChangePreviousWeapon);
	// EŰ: ���� ����� ����
	InputComponent->BindAction("ChangeNextWeapon", IE_Pressed, this, &APioneerController::ChangeNextWeapon);
	// ���콺 ��Ŭ��: ���� <--> ��������
	InputComponent->BindAction("ArmOrDisArmWeapon", IE_Pressed, this, &APioneerController::ArmOrDisArmWeapon);

	// BŰ: �ǹ��Ǽ���� ����
	InputComponent->BindAction("ConstructingMode", IE_Pressed, this, &APioneerController::ConstructingMode);
	// ESCŰ: �ǹ��Ǽ���� ���
	InputComponent->BindAction("ESC_ConstructingMode", IE_Pressed, this, &APioneerController::ESC_ConstructingMode);
	// 1~9Ű: �� Ű�� �ش��ϴ� �ǹ� ����
	InputComponent->BindAxis("SpawnBuilding", this, &APioneerController::SpawnBuilding);
	// Q EŰ: �ǹ��Ǽ���忡�� �ǹ� ȸ��
	InputComponent->BindAxis("RotatingBuilding", this, &APioneerController::RotatingBuilding);
	// ���콺 ��Ŭ��: �ǹ��Ǽ���带 ������ �ǹ��� �Ǽ�
	InputComponent->BindAction("PlaceBuilding", IE_Pressed, this, &APioneerController::PlaceBuilding);

	// F10Ű: �޴�
	InputComponent->BindAction("Menu", IE_Pressed, this, &APioneerController::Menu);
	// ShiftŰ: ���ھ� ����
	InputComponent->BindAxis("ScoreBoard", this, &APioneerController::ScoreBoard);

	// Left ArrowŰ: ���� ����
	InputComponent->BindAction("Observing_Left", IE_Pressed, this, &APioneerController::ObservingLeft);
	// Right ArrowŰ: ���� ����
	InputComponent->BindAction("Observing_Right", IE_Pressed, this, &APioneerController::ObservingRight);
	// Space BarŰ: ���� ��������
	InputComponent->BindAction("Observing_Free", IE_Pressed, this, &APioneerController::ObservingFree);
	// EnterŰ: ���� �������� AI Pioneer�� ����
	InputComponent->BindAction("Observing_Possess", IE_Pressed, this, &APioneerController::ObservingPossess);

	// �������� ī�޶� W SŰ: �������̵�
	InputComponent->BindAxis("FreeViewPoint_MoveForward", this, &APioneerController::FreeViewPoint_MoveForward);
	// �������� ī�޶� A DŰ: �¿����̵�
	InputComponent->BindAxis("FreeViewPoint_MoveRight", this, &APioneerController::FreeViewPoint_MoveRight);
	// �������� ī�޶� Q EŰ: �����̵�
	InputComponent->BindAxis("FreeViewPoint_MoveUp", this, &APioneerController::FreeViewPoint_MoveUp);

	// (�̽��Ϳ���)Ctrl + ZŰ: ���� ������ ���ݼӵ� ����
	InputComponent->BindAxis("EasterEgg", this, &APioneerController::EasterEgg);

}

void APioneerController::SetViewTargetWithBlend(class AActor* NewViewTarget, float BlendTime, enum EViewTargetBlendFunction BlendFunc, float BlendExp, bool bLockOutgoing)
{
	APlayerController::SetViewTargetWithBlend(NewViewTarget, BlendTime, BlendFunc, BlendExp, bLockOutgoing);
}

void APioneerController::OnPossess(APawn* InPawn)
{
	if (!InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::OnPossess(...)> if (!InPawn)"));
		return;
	}

	if (APioneer* pioneer = dynamic_cast<APioneer*>(InPawn))
	{
		Super::OnPossess(InPawn);
		Pioneer = pioneer;

		bObservation = false;
	}
}
void APioneerController::OnUnPossess()
{
	Pioneer = nullptr;

	if (!GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::OnUnPossess(...)> if (!GetPawn())"));
		return;
	}

	Super::OnUnPossess();

	SetPawn(nullptr);

	bObservation = true;
}

void APioneerController::MoveToMouseCursor()
{
	if (!Pioneer)
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::MoveToMouseCursor()> if (!Pioneer)"));
		return;
	}

	if (APioneer* pioneer = Cast<APioneer>(GetPawn()))
	{
		if (pioneer->GetCursorToWorld() == nullptr)
			return;
		if (pioneer->bDying == true)
			return;

		PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, pioneer->GetCursorToWorld()->GetComponentLocation());
	}
}

void APioneerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}
void APioneerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void APioneerController::MoveForward(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.�մϴ�.
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::MoveForward(...)> if (!Pioneer || !GetPawn())"));
		return;
	}
	
	if (Pioneer->bDying)
		return;

	// Navigation���� �̵��ϴ� ���̾����� ����ϴ�. 
	StopMovement();

	//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // ��Ʈ�ѷ��� ȸ������ �����ɴϴ�.
	const FRotator Rotation = Pioneer->CameraBoomRotation; // Pioneer�� ī�޶� ȸ������ �����ɴϴ�.
	const FRotator YawRotation(0, Rotation.Yaw, 0); // ������ ������ ã���ϴ�.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	Pioneer->AddMovementInput(Direction, Value); // �ش� �������� �̵� ���� �߰��մϴ�.
}
void APioneerController::MoveRight(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::MoveRight(...)> if (!Pioneer || !GetPawn())"));
		return;
	}
	
	if (Pioneer->bDying)
		return;

	// Navigation���� �̵��ϴ� ���̾����� ����ϴ�. 
	StopMovement();

	//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // ��Ʈ�ѷ��� ȸ������ �����ɴϴ�.
	const FRotator Rotation = Pioneer->CameraBoomRotation; // Pioneer�� ī�޶� ȸ������ �����ɴϴ�.
	const FRotator YawRotation(0, Rotation.Yaw, 0); // ������ ������ ã���ϴ�.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	Pioneer->AddMovementInput(Direction, Value); // �ش� �������� �̵� ���� �߰��մϴ�.
}

void APioneerController::ZoomInOrZoomOut(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.
	if (Value == 0.0f)
		return;

	// ���ھ������ ��Ÿ�������� �������� �ʽ��ϴ�.
	if (bScoreBoard)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ZoomInOrZoomOut(...)> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	Pioneer->ZoomInOrZoomOut(Value);
}

void APioneerController::FireWeapon(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::FireWeapon(...)> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	Pioneer->FireWeapon();
}
void APioneerController::ChangePreviousWeapon()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ChangePreviousWeapon()> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	// �Ǽ����� �������� �ʽ��ϴ�.
	if (Pioneer->bConstructingMode)
		return;
		
	Pioneer->ChangeWeapon(-1);
}
void APioneerController::ChangeNextWeapon()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ChangeNextWeapon()> if (!Pioneer || !GetPawn())"));
		return;
	}
	
	if (Pioneer->bDying)
		return;

	// �Ǽ����� �������� �ʽ��ϴ�.
	if (Pioneer->bConstructingMode)
		return;

	Pioneer->ChangeWeapon(1);
}
void APioneerController::ArmOrDisArmWeapon()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ArmOrDisArmWeapon()> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	// �Ǽ���� ����
	if (Pioneer->bConstructingMode)
		Pioneer->bConstructingMode = false;

	if (Pioneer->GetCurrentWeapon())
	{
		Pioneer->Disarming();
		Pioneer->bArmedWeapon = false;
	}
	else
	{
		Pioneer->Arming();
		Pioneer->bArmedWeapon = true;
	}

	Pioneer->DestroyBuilding();
}

void APioneerController::ConstructingMode()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ConstructingMode()> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	Pioneer->Disarming();
	Pioneer->bArmedWeapon = false;

	Pioneer->bConstructingMode = true;
}
void APioneerController::ESC_ConstructingMode()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ESC_ConstructingMode()> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	// �Ǽ����� ���¶��
	if (Pioneer->bConstructingMode)
	{
		Pioneer->DestroyBuilding();
		Pioneer->bConstructingMode = false;
	}
}
void APioneerController::SpawnBuilding(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::SpawnBuilding(...)> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	if (Pioneer->bConstructingMode)
		Pioneer->SpawnBuilding((int)Value);
}
void APioneerController::RotatingBuilding(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::RotatingBuilding(...)> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	Pioneer->RotatingBuilding(Value * 128.0f * PlayTickDeltaTime);
}
void APioneerController::PlaceBuilding()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::PlaceBuilding()> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	// nullptr üũ
	if (!Pioneer->GetBuilding())
		return;

	// �⺻������ �ڿ��� �����ϸ� �Ǽ����� �ʽ��ϴ�.
	if (APioneerManager::Resources.NumOfMineral < Pioneer->GetBuilding()->NeedMineral
		|| APioneerManager::Resources.NumOfOrganic < Pioneer->GetBuilding()->NeedOrganicMatter)
	{
		return;
	}

	Pioneer->PlaceBuilding();
}

void APioneerController::Menu()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::Menu()> if (!world)"));
		return;
	}

	if (AInGameMode* inGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(world)))
	{
		inGameMode->ToggleInGameMenuWidget();
	}
}
void APioneerController::ScoreBoard(float Value)
{
	// Value�� ���� ������ Ű�� �������� �������Դϴ�.
	if (Value == 0.0f)
	{
		if (bScoreBoard)
		{
			UWorld* const world = GetWorld();
			if (!world)
			{
				UE_LOG(LogTemp, Error, TEXT("<APioneerController::ScoreBoard(...)> if (!world)"));	
				return;
			}

			if (AInGameMode* inGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(world)))
			{
				inGameMode->DeactivateInGameScoreBoardWidget();
			}

			bScoreBoard = false;
		}

		return;
	}
	/************************************************************/

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ScoreBoard(...)> if (!world)"));	
		return;
	}

	if (AInGameMode* inGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(world)))
	{
		inGameMode->ActivateInGameScoreBoardWidget();
	}

	bScoreBoard = true;
}

void APioneerController::ObservingLeft()
{
	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ObservingLeft()> if (!PioneerManager)"));
		return;
	}

	// ������� �����϶��� �����մϴ�.
	if (PioneerManager->ViewpointState == EViewpointState::Observation)
	{
		PioneerManager->ObserveLeft();
	}
}
void APioneerController::ObservingRight()
{
	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ObservingRight()> if (!PioneerManager)"));
		return;
	}

	// ������� �����϶��� �����մϴ�.
	if (PioneerManager->ViewpointState == EViewpointState::Observation)
	{
		PioneerManager->ObserveRight();
	}
}
void APioneerController::ObservingFree()
{
	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ObservingFree()> if (!PioneerManager)"));
		return;
	}

	// ������� �����϶��� �����մϴ�.
	if (PioneerManager->ViewpointState == EViewpointState::Idle ||
		PioneerManager->ViewpointState == EViewpointState::Observation)
	{
		PioneerManager->SwitchToFreeViewpoint();
	}
	else if (PioneerManager->ViewpointState == EViewpointState::Free)
	{
		PioneerManager->Observation();
	}
}
void APioneerController::ObservingPossess()
{
	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ObservingPossess()> if (!PioneerManager)"));
		return;
	}

	// ������� �����϶��� �����մϴ�.
	if (PioneerManager->ViewpointState == EViewpointState::Observation)
	{
		PioneerManager->PossessObservingPioneer();
	}
}

void APioneerController::FreeViewPoint_MoveForward(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.
	if (Value == 0.0f)
		return;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::FreeViewPoint_MoveForward(...)> if (!PioneerManager)"));
		return;
	}

	// �������� ����϶��� �����մϴ�.
	if (PioneerManager->ViewpointState != EViewpointState::Free)
	{
		return;
	}

	if (AWorldViewCameraActor* freeViewCamera = PioneerManager->GetFreeViewCamera())
	{
		FTransform transform = freeViewCamera->GetActorTransform();
		transform.AddToTranslation(freeViewCamera->GetActorForwardVector() * Value * 1024.0f * PlayTickDeltaTime);
		freeViewCamera->SetActorTransform(transform);
	}
}
void APioneerController::FreeViewPoint_MoveRight(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.
	if (Value == 0.0f)
		return;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::FreeViewPoint_MoveRight(...)> if (!PioneerManager)"));
		return;
	}

	// �������� ����϶��� �����մϴ�.
	if (PioneerManager->ViewpointState != EViewpointState::Free)
	{
		return;
	}

	if (AWorldViewCameraActor* freeViewCamera = PioneerManager->GetFreeViewCamera())
	{
		FTransform transform = freeViewCamera->GetActorTransform();
		transform.AddToTranslation(freeViewCamera->GetActorRightVector() * Value * 1024.0f * PlayTickDeltaTime);
		freeViewCamera->SetActorTransform(transform);
	}
}
void APioneerController::FreeViewPoint_MoveUp(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.
	if (Value == 0.0f)
		return;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::FreeViewPoint_MoveUp(...)> if (!PioneerManager)"));
		return;
	}

	// �������� ����϶��� �����մϴ�.
	if (PioneerManager->ViewpointState != EViewpointState::Free)
	{
		return;
	}

	if (AWorldViewCameraActor* freeViewCamera = PioneerManager->GetFreeViewCamera())
	{
		FVector moveUp = freeViewCamera->GetActorUpVector() * Value * 1024.0f * PlayTickDeltaTime;
		FVector movedLocation = freeViewCamera->GetActorLocation() + moveUp;
		freeViewCamera->SetActorLocation(movedLocation);
	}
}

void APioneerController::EasterEgg(float Value)
{	
	// Value�� ���� ������ ������ �ʿ䰡 ������ �����մϴ�.
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::EasterEgg()> if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->bDying)
		return;

	if (Pioneer->GetCurrentWeapon())
		Pioneer->GetCurrentWeapon()->AttackSpeed++;

	Pioneer->MoveSpeed += 0.25f;
	if (Pioneer->GetCharacterMovement())
		Pioneer->GetCharacterMovement()->MaxWalkSpeed = AInGameMode::CellSize * Pioneer->MoveSpeed; // ������ �� �ȴ� �ӵ�
}

void APioneerController::SetPioneerManager(class APioneerManager* PM)
{
	this->PioneerManager = PM;
}

