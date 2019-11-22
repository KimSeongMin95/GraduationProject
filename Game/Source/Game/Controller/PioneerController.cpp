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

	InputComponent->BindAction("Fire", IE_Pressed, this, &APioneerController::FireWeapon);

	// 임시
	InputComponent->BindAction("CW", IE_Pressed, this, &APioneerController::ChangeWeapon);
		

	InputComponent->BindAxis("RotatingBuilding", this, &APioneerController::RotatingBuilding);
	InputComponent->BindAction("PlaceBuilding", IE_Pressed, this, &APioneerController::PlaceBuilding);
	InputComponent->BindAction("ConstructingMode", IE_Pressed, this, &APioneerController::ConstructingMode);
}

//void APioneerController::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}

/** Navigate player to the current mouse cursor location. */
void APioneerController::MoveToMouseCursor()
{
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

/** Navigate player to the current touch location. */
void APioneerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// 받아온 터치한 위치를 ScreenSpaceLocation에 저장합니다.
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	// 터치한 위치 아래에 무엇이 존재하는지 추적합니다.
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);

	// Indicates if this hit was a result of blocking collision
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		// Hit.ImpactPoint는 Ray와 충돌한 물체의 표면 지점을 반환합니다.
		PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, HitResult.ImpactPoint);
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
	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	if (MyPawn && (Value != 0.0f))
	{
		//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // 컨트롤러의 회전값을 가져옵니다.
		const FRotator Rotation = MyPawn->CameraBoomRotation; // Pioneer의 카메라 회전값을 가져옵니다.
		const FRotator YawRotation(0, Rotation.Yaw, 0); // 오른쪽 방향을 찾습니다.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		MyPawn->AddMovementInput(Direction, Value); // 해당 방향으로 이동 값을 추가합니다.

		//// 방향을 고정합니다.
		//MyPawn->AddMovementInput(FVector().ForwardVector, value);
	}
}

void APioneerController::MoveRight(float Value)
{
	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	if (MyPawn && (Value != 0.0f))
	{
		//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // 컨트롤러의 회전값을 가져옵니다.
		const FRotator Rotation = MyPawn->CameraBoomRotation; // Pioneer의 카메라 회전값을 가져옵니다.
		const FRotator YawRotation(0, Rotation.Yaw, 0); // 오른쪽 방향을 찾습니다.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		MyPawn->AddMovementInput(Direction, Value); // 해당 방향으로 이동 값을 추가합니다.

		//// 방향을 고정합니다.
		//MyPawn->AddMovementInput(FVector().RightVector, value);
	}
}

void APioneerController::ZoomInOrZoomOut(float Value)
{
	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	if (MyPawn && (Value != 0.0f))
	{
		MyPawn->ZoomInOrZoomOut(Value);
	}
}

void APioneerController::FireWeapon()
{
	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	if (MyPawn)
	{
		MyPawn->FireWeapon();
	}
}

void APioneerController::ChangeWeapon()
{
	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	if (MyPawn)
	{
		if (MyPawn->bConstructingMode == false)
			MyPawn->ChangeWeapon();
		else
			MyPawn->ChangeBuilding();
	}
}

void APioneerController::RotatingBuilding(float Value)
{
	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	if (MyPawn)
	{
		MyPawn->RotatingBuilding(Value);
	}
}

void APioneerController::PlaceBuilding()
{
	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	if (MyPawn)
	{
		MyPawn->PlaceBuilding();
	}
}

void APioneerController::ConstructingMode()
{
	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	if (MyPawn)
	{
		MyPawn->bConstructingMode = 1 - MyPawn->bConstructingMode;
		if (MyPawn->bConstructingMode)
			MyPawn->SpawnBuilding();
		else
			MyPawn->DestroyBuilding();
		MyPawn->Disarming();
	}
}

void APioneerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void APioneerController::OnUnPossess()
{
	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());
	
	Super::OnUnPossess();

	SetPawn(nullptr);

	// AIController()에게 빙의하도록 합니다.
	if (MyPawn)
		MyPawn->PossessAIController();
	
}