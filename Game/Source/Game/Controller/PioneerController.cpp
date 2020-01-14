// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerController.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
#include "PathFinding.h"
#include "Controller/PioneerAIController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

APioneerController::APioneerController()
{
	// setting mouse
	bShowMouseCursor = true; // 마우스를 보이게 합니다.
	//DefaultMouseCursor = EMouseCursor::Default; // EMouseCursor::에 따라 마우스 커서 모양을 변경할 수 있습니다.
	DefaultMouseCursor = EMouseCursor::Crosshairs; // EMouseCursor::에 따라 마우스 커서 모양을 변경할 수 있습니다.
}

void APioneerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

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
	
	InputComponent->BindAxis("MoveForward", this, &APioneerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APioneerController::MoveRight);

	InputComponent->BindAxis("ZoomInOut", this, &APioneerController::ZoomInOrZoomOut);

	InputComponent->BindAxis("FireWeapon", this, &APioneerController::FireWeapon);
	InputComponent->BindAction("ChangePreviousWeapon", IE_Pressed, this, &APioneerController::ChangePreviousWeapon);
	InputComponent->BindAction("ChangeNextWeapon", IE_Pressed, this, &APioneerController::ChangeNextWeapon);
	InputComponent->BindAction("ArmOrDisArmWeapon", IE_Pressed, this, &APioneerController::ArmOrDisArmWeapon);

	// E키: 바닥에 있는 아이템 줍기
	// G키: 현재 무기를 바닥에 버리기


	InputComponent->BindAxis("RotatingBuilding", this, &APioneerController::RotatingBuilding);
	InputComponent->BindAction("PlaceBuilding", IE_Pressed, this, &APioneerController::PlaceBuilding);

}

//void APioneerController::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}

/** Navigate player to the current mouse cursor location. */
void APioneerController::MoveToMouseCursor()
{
	if (!Pioneer)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::MoveToMouseCursor: if (!Pioneer)"));
		return;
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
		if (APioneer* MyPawn = Cast<APioneer>(GetPawn()))
		{
			if (MyPawn->CursorToWorld == nullptr)
				return;

			// 죽으면 함수를 실행하지 않음.
			if (MyPawn->bDying == true)
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
			PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, MyPawn->CursorToWorld->GetComponentLocation());
		}
	}
}

///** Navigate player to the current touch location. */
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
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::MoveForward: if (!Pioneer || !GetPawn())"));
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
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::MoveRight: if (!Pioneer || !GetPawn())"));
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

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::ZoomInOrZoomOut: if (!Pioneer || !GetPawn())"));
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
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::FireWeapon: if (!Pioneer || !GetPawn())"));
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
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::ChangeWeapon: if (!Pioneer || !GetPawn())"));
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
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::ChangeWeapon: if (!Pioneer || !GetPawn())"));
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
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::ConstructingMode: if (!Pioneer || !GetPawn())"));
		return;
	}


	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	if (Pioneer->CurrentWeapon)
		Pioneer->Disarming();
	else
		Pioneer->Arming();
}


void APioneerController::RotatingBuilding(float Value)
{
	// Value에 값이 없으면 실행할 필요가 없으니 종료
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::RotatingBuilding: if (!Pioneer || !GetPawn())"));
		return;
	}


	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	Pioneer->RotatingBuilding(Value);
}

void APioneerController::PlaceBuilding()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::PlaceBuilding: if (!Pioneer || !GetPawn())"));
		return;
	}


	// 죽으면 함수를 실행하지 않음.
	if (Pioneer->bDying)
		return;

	Pioneer->PlaceBuilding();
}









//void APioneerController::ConstructingMode()
//{
//	if (!Pioneer || !GetPawn())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("APioneerController::ConstructingMode: if (!Pioneer || !GetPawn())"));
//		return;
//	}
//
//
//	// 죽으면 함수를 실행하지 않음.
//	if (Pioneer->bDying)
//		return;
//
//	Pioneer->bConstructingMode = 1 - Pioneer->bConstructingMode;
//
//	if (Pioneer->bConstructingMode)
//		Pioneer->SpawnBuilding();
//	else
//		Pioneer->DestroyBuilding();
//
//	Pioneer->Disarming();
//	
//}

void APioneerController::OnPossess(APawn* InPawn)
{
	if (!InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::OnPossess: !InPawn"));
		return;
	}

	if (APioneer* pioneer = dynamic_cast<APioneer*>(InPawn))
	{
		Super::OnPossess(InPawn);
		Pioneer = pioneer;
	}
}

void APioneerController::OnUnPossess()
{
	Pioneer = nullptr;

	if (!GetPawn())
	{	
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::OnUnPossess: !GetPawn()"));
		return;
	}

	Super::OnUnPossess();
	SetPawn(nullptr);
}