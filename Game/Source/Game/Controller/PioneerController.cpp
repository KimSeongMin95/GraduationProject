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

	bShowMouseCursor = true; // 마우스를 보이게 합니다.
	//DefaultMouseCursor = EMouseCursor::Default; // EMouseCursor::에 따라 마우스 커서 모양을 변경할 수 있습니다.
	DefaultMouseCursor = EMouseCursor::Crosshairs; // EMouseCursor::에 따라 마우스 커서 모양을 변경할 수 있습니다.

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
	
	// W S키: 전후진이동
	InputComponent->BindAxis("MoveForward", this, &APioneerController::MoveForward);
	// A D키: 좌우측이동
	InputComponent->BindAxis("MoveRight", this, &APioneerController::MoveRight);

	// 마우스 휠 위아래: 카메라 줌인&줌아웃
	InputComponent->BindAxis("ZoomInOut", this, &APioneerController::ZoomInOrZoomOut);

	// 마우스 좌클릭: 무기 발사
	InputComponent->BindAxis("FireWeapon", this, &APioneerController::FireWeapon);
	// Q키: 이전 무기로 변경
	InputComponent->BindAction("ChangePreviousWeapon", IE_Pressed, this, &APioneerController::ChangePreviousWeapon);
	// E키: 다음 무기로 변경
	InputComponent->BindAction("ChangeNextWeapon", IE_Pressed, this, &APioneerController::ChangeNextWeapon);
	// 마우스 휠클릭: 무장 <--> 무장해제
	InputComponent->BindAction("ArmOrDisArmWeapon", IE_Pressed, this, &APioneerController::ArmOrDisArmWeapon);

	// B키: 건물건설모드 진입
	InputComponent->BindAction("ConstructingMode", IE_Pressed, this, &APioneerController::ConstructingMode);
	// ESC키: 건물건설모드 취소
	InputComponent->BindAction("ESC_ConstructingMode", IE_Pressed, this, &APioneerController::ESC_ConstructingMode);
	// 1~9키: 각 키에 해당하는 건물 스폰
	InputComponent->BindAxis("SpawnBuilding", this, &APioneerController::SpawnBuilding);
	// Q E키: 건물건설모드에서 건물 회전
	InputComponent->BindAxis("RotatingBuilding", this, &APioneerController::RotatingBuilding);
	// 마우스 좌클릭: 건물건설모드를 끝내고 건물을 건설
	InputComponent->BindAction("PlaceBuilding", IE_Pressed, this, &APioneerController::PlaceBuilding);

	// F10키: 메뉴
	InputComponent->BindAction("Menu", IE_Pressed, this, &APioneerController::Menu);
	// Shift키: 스코어 보드
	InputComponent->BindAxis("ScoreBoard", this, &APioneerController::ScoreBoard);

	// Left Arrow키: 관전 변경
	InputComponent->BindAction("Observing_Left", IE_Pressed, this, &APioneerController::ObservingLeft);
	// Right Arrow키: 관전 변경
	InputComponent->BindAction("Observing_Right", IE_Pressed, this, &APioneerController::ObservingRight);
	// Space Bar키: 관전 자유시점
	InputComponent->BindAction("Observing_Free", IE_Pressed, this, &APioneerController::ObservingFree);
	// Enter키: 현재 관전중인 AI Pioneer에 빙의
	InputComponent->BindAction("Observing_Possess", IE_Pressed, this, &APioneerController::ObservingPossess);

	// 자유시점 카메라 W S키: 전후진이동
	InputComponent->BindAxis("FreeViewPoint_MoveForward", this, &APioneerController::FreeViewPoint_MoveForward);
	// 자유시점 카메라 A D키: 좌우측이동
	InputComponent->BindAxis("FreeViewPoint_MoveRight", this, &APioneerController::FreeViewPoint_MoveRight);
	// 자유시점 카메라 Q E키: 상하이동
	InputComponent->BindAxis("FreeViewPoint_MoveUp", this, &APioneerController::FreeViewPoint_MoveUp);

	// (이스터에그)Ctrl + Z키: 현재 무기의 공격속도 증가
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.합니다.
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::MoveForward(...)> if (!Pioneer || !GetPawn())"));
		return;
	}
	
	if (Pioneer->bDying)
		return;

	// Navigation으로 이동하던 중이었으면 멈춥니다. 
	StopMovement();

	//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // 컨트롤러의 회전값을 가져옵니다.
	const FRotator Rotation = Pioneer->CameraBoomRotation; // Pioneer의 카메라 회전값을 가져옵니다.
	const FRotator YawRotation(0, Rotation.Yaw, 0); // 오른쪽 방향을 찾습니다.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	Pioneer->AddMovementInput(Direction, Value); // 해당 방향으로 이동 값을 추가합니다.
}
void APioneerController::MoveRight(float Value)
{
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::MoveRight(...)> if (!Pioneer || !GetPawn())"));
		return;
	}
	
	if (Pioneer->bDying)
		return;

	// Navigation으로 이동하던 중이었으면 멈춥니다. 
	StopMovement();

	//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // 컨트롤러의 회전값을 가져옵니다.
	const FRotator Rotation = Pioneer->CameraBoomRotation; // Pioneer의 카메라 회전값을 가져옵니다.
	const FRotator YawRotation(0, Rotation.Yaw, 0); // 오른쪽 방향을 찾습니다.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	Pioneer->AddMovementInput(Direction, Value); // 해당 방향으로 이동 값을 추가합니다.
}

void APioneerController::ZoomInOrZoomOut(float Value)
{
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.
	if (Value == 0.0f)
		return;

	// 스코어보드판이 나타나있으면 실행하지 않습니다.
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.
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

	// 건설모드면 실행하지 않습니다.
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

	// 건설모드면 실행하지 않습니다.
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

	// 건설모드 해제
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

	// 건설중인 상태라면
	if (Pioneer->bConstructingMode)
	{
		Pioneer->DestroyBuilding();
		Pioneer->bConstructingMode = false;
	}
}
void APioneerController::SpawnBuilding(float Value)
{
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.
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

	// nullptr 체크
	if (!Pioneer->GetBuilding())
		return;

	// 기본적으로 자원이 부족하면 건설하지 않습니다.
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
	// Value에 값이 없으면 키가 눌려있지 않은것입니다.
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

	// 관전모드 상태일때만 실행합니다.
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

	// 관전모드 상태일때만 실행합니다.
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

	// 관전모드 상태일때만 실행합니다.
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

	// 관전모드 상태일때만 실행합니다.
	if (PioneerManager->ViewpointState == EViewpointState::Observation)
	{
		PioneerManager->PossessObservingPioneer();
	}
}

void APioneerController::FreeViewPoint_MoveForward(float Value)
{
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.
	if (Value == 0.0f)
		return;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::FreeViewPoint_MoveForward(...)> if (!PioneerManager)"));
		return;
	}

	// 자유시점 모드일때만 조종합니다.
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.
	if (Value == 0.0f)
		return;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::FreeViewPoint_MoveRight(...)> if (!PioneerManager)"));
		return;
	}

	// 자유시점 모드일때만 조종합니다.
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.
	if (Value == 0.0f)
		return;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::FreeViewPoint_MoveUp(...)> if (!PioneerManager)"));
		return;
	}

	// 자유시점 모드일때만 조종합니다.
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료합니다.
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
		Pioneer->GetCharacterMovement()->MaxWalkSpeed = AInGameMode::CellSize * Pioneer->MoveSpeed; // 움직일 때 걷는 속도
}

void APioneerController::SetPioneerManager(class APioneerManager* PM)
{
	this->PioneerManager = PM;
}

