// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerController.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "PathFinding.h"

#include "Character/Pioneer.h"
#include "Controller/PioneerAIController.h"

#include "Item/Item.h"
#include "Item/Weapon/Weapon.h"

#include "GameMode/TutorialGameMode.h"
#include "GameMode/OnlineGameMode.h"

#include "PioneerManager.h"

#include "Etc/WorldViewCameraActor.h"

#include "Building/Building.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
APioneerController::APioneerController()
{
	Pioneer = nullptr;

	bScoreBoard = false;

	PioneerManager = nullptr;

	bObservation = true;

	// setting mouse
	bShowMouseCursor = true; // 마우스를 보이게 합니다.
	//DefaultMouseCursor = EMouseCursor::Default; // EMouseCursor::에 따라 마우스 커서 모양을 변경할 수 있습니다.
	DefaultMouseCursor = EMouseCursor::Crosshairs; // EMouseCursor::에 따라 마우스 커서 모양을 변경할 수 있습니다.

	PlayTickDeltaTime = 0.0f;
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

	//// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APioneerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APioneerController::MoveToTouchLocation);

	//// support VR
	//InputComponent->BindAction("ResetVR", IE_Pressed, this, &APioneerController::OnResetVR);
	
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
/*** Basic Function : End ***/


/*** APlayerController : Start ***/
void APioneerController::OnPossess(APawn* InPawn)
{
	if (!InPawn)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::OnPossess(...)> if (!InPawn)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::OnUnPossess(...)> if (!GetPawn())"));
#endif		
		return;
	}

	Super::OnUnPossess();

	SetPawn(nullptr);

	bObservation = true;
}
/*** APlayerController : End ***/


/*** APioneerController : Start ***/
void APioneerController::MoveToMouseCursor()
{
	if (!Pioneer)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::MoveToMouseCursor()> if (!Pioneer)"));
#endif		return;
	}

	// VR 사용시
	/*if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (APioneer* MyPawn = Cast<APioneer>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else*/
	// 마우스 커서 사용시
	{
		if (APioneer* pioneer = Cast<APioneer>(GetPawn()))
		{
			if (pioneer->GetCursorToWorld() == nullptr)
				return;

			// 죽으면 함수를 실행하지 않음.
			if (pioneer->bDying == true)
				return;

			//// Trace to see what is under the mouse cursor
			//// 마우스 커서 아래에 무엇이 존재하는지 추적합니다.
			//FHitResult HitResult;
			//GetHitResultUnderCursor(ECC_Visibility, false, HitResult); // 내부에서 APlayerController::GetHitResultAtScreenPosition(...)을 사용합니다.
			//// ECollisionChannel:: indicating different type of objects for rigid-body collision purposes.

			//// Indicates if this hit was a result of blocking collision
			//if (HitResult.bBlockingHit)
			//{
			//	// We hit something, move there
			//	// Hit.ImpactPoint는 Ray와 충돌한 물체의 표면 지점을 반환합니다.
			//	PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, HitResult.ImpactPoint);
			//}
			PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, pioneer->GetCursorToWorld()->GetComponentLocation());
		}
	}
}

//void APioneerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	// 받아온 터치한 위치를 ScreenSpaceLocation에 저장합니다.
//	FVector2D ScreenSpaceLocation(Location);
//
//	// Trace to see what is under the touch location
//	// 터치한 위치 아래에 무엇이 존재하는지 추적합니다.
//	FHitResult HitResult;
//	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
//
//	// Indicates if this hit was a result of blocking collision
//	if (HitResult.bBlockingHit)
//	{
//		// We hit something, move there
//		// Hit.ImpactPoint는 Ray와 충돌한 물체의 표면 지점을 반환합니다.
//		PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, HitResult.ImpactPoint);
//	}
//}

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
	//// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	//APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::MoveForward(...)> if (!Pioneer || !GetPawn())"));
#endif		
		return;
	}
	
	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	// Navigation으로 이동하던 중이었으면 멈춥니다. 
	StopMovement();

	//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // 컨트롤러의 회전값을 가져옵니다.
	const FRotator Rotation = Pioneer->CameraBoomRotation; // Pioneer의 카메라 회전값을 가져옵니다.
	const FRotator YawRotation(0, Rotation.Yaw, 0); // 오른쪽 방향을 찾습니다.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	Pioneer->AddMovementInput(Direction, Value); // 해당 방향으로 이동 값을 추가합니다.

	//// 방향을 고정합니다.
	//MyPawn->AddMovementInput(FVector().ForwardVector, value);
}

void APioneerController::MoveRight(float Value)
{
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::MoveRight(...)> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}


	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	// Navigation으로 이동하던 중이었으면 멈춥니다. 
	StopMovement();

	//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // 컨트롤러의 회전값을 가져옵니다.
	const FRotator Rotation = Pioneer->CameraBoomRotation; // Pioneer의 카메라 회전값을 가져옵니다.
	const FRotator YawRotation(0, Rotation.Yaw, 0); // 오른쪽 방향을 찾습니다.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	Pioneer->AddMovementInput(Direction, Value); // 해당 방향으로 이동 값을 추가합니다.

	//// 방향을 고정합니다.
	//MyPawn->AddMovementInput(FVector().RightVector, value);
}

void APioneerController::ZoomInOrZoomOut(float Value)
{
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	// 스코어보드판이 나타나있으면 실행하지 않습니다.
	if (bScoreBoard)
		return;

	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ZoomInOrZoomOut(...)> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}


	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	Pioneer->ZoomInOrZoomOut(Value);
}
void APioneerController::FireWeapon(float Value)
{
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::FireWeapon(...)> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}


	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	Pioneer->FireWeapon();
}

void APioneerController::ChangePreviousWeapon()
{
	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ChangePreviousWeapon()> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}


	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	// 건설모드면 실행하지 않음.
	if (Pioneer->bConstructingMode)
		return;
		
	Pioneer->ChangeWeapon(-1);
}
void APioneerController::ChangeNextWeapon()
{
	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ChangeNextWeapon()> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}


	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	// 건설모드면 실행하지 않음.
	if (Pioneer->bConstructingMode)
		return;

	Pioneer->ChangeWeapon(1);
}
void APioneerController::ArmOrDisArmWeapon()
{
	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ArmOrDisArmWeapon()> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}


	// 죽으면 함수를 실행하지 않음.
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ConstructingMode()> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}

	// 죽으면 함수를 실행하지 않음.
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::ESC_ConstructingMode()> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}

	// 죽으면 함수를 실행하지 않음.
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::SpawnBuilding(...)> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}

	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	if (Pioneer->bConstructingMode)
		Pioneer->SpawnBuilding((int)Value);
}
void APioneerController::RotatingBuilding(float Value)
{
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::RotatingBuilding(...)> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}


	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	Pioneer->RotatingBuilding(Value * 128.0f * PlayTickDeltaTime);
}
void APioneerController::PlaceBuilding()
{
	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::PlaceBuilding()> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}


	// 죽으면 함수를 실행하지 않음.
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::Menu()> if (!world)"));
#endif		
		return;
	}

	ATutorialGameMode* tutorialGameMode = Cast<ATutorialGameMode>(UGameplayStatics::GetGameMode(world));
	AOnlineGameMode* onlineGameMode = Cast<AOnlineGameMode>(UGameplayStatics::GetGameMode(world));

	if (tutorialGameMode)
	{
		tutorialGameMode->ToggleInGameMenuWidget();
	}
	else if (onlineGameMode)
	{
		onlineGameMode->ToggleInGameMenuWidget();
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
				UE_LOG(LogTemp, Error, TEXT("<APioneerController::ScoreBoard(...)> if (!world)"));
#endif				
				return;
			}

			ATutorialGameMode* tutorialGameMode = Cast<ATutorialGameMode>(UGameplayStatics::GetGameMode(world));
			AOnlineGameMode* onlineGameMode = Cast<AOnlineGameMode>(UGameplayStatics::GetGameMode(world));

			if (tutorialGameMode)
			{

			}
			else if (onlineGameMode)
			{
				onlineGameMode->DeactivateInGameScoreBoardWidget();
			}


			bScoreBoard = false;
		}

		return;
	}

	/************************************************************/

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ScoreBoard(...)> if (!world)"));
#endif		
		return;
	}

	ATutorialGameMode* tutorialGameMode = Cast<ATutorialGameMode>(UGameplayStatics::GetGameMode(world));
	AOnlineGameMode* onlineGameMode = Cast<AOnlineGameMode>(UGameplayStatics::GetGameMode(world));

	if (tutorialGameMode)
	{

	}
	else if (onlineGameMode)
	{
		onlineGameMode->ActivateInGameScoreBoardWidget();
	}

	bScoreBoard = true;
}

void APioneerController::ObservingLeft()
{
	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ObservingLeft()> if (!PioneerManager)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ObservingRight()> if (!PioneerManager)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ObservingFree()> if (!PioneerManager)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::ObservingPossess()> if (!PioneerManager)"));
#endif		
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::FreeViewPoint_MoveForward(...)> if (!PioneerManager)"));
#endif		
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::FreeViewPoint_MoveRight(...)> if (!PioneerManager)"));
#endif		
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerController::FreeViewPoint_MoveUp(...)> if (!PioneerManager)"));
#endif		
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
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerController::EasterEgg()> if (!Pioneer || !GetPawn())"));
#endif
		return;
	}

	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	if (Pioneer->GetCurrentWeapon())
		Pioneer->GetCurrentWeapon()->AttackSpeed++;

	Pioneer->MoveSpeed += 0.25f;
	if (Pioneer->GetCharacterMovement())
		Pioneer->GetCharacterMovement()->MaxWalkSpeed = AOnlineGameMode::CellSize * Pioneer->MoveSpeed; // 움직일 때 걷는 속도

}



void APioneerController::SetPioneerManager(class APioneerManager* PM)
{
	this->PioneerManager = PM;
}

void APioneerController::SetViewTargetWithBlend(class AActor* NewViewTarget, float BlendTime, enum EViewTargetBlendFunction BlendFunc, float BlendExp, bool bLockOutgoing)
{
	APlayerController::SetViewTargetWithBlend(NewViewTarget, BlendTime, BlendFunc, BlendExp, bLockOutgoing);
}

/*** APioneerController : End ***/



//void APioneerController::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}