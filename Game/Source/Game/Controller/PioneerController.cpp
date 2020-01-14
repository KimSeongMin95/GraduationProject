// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerController.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Pioneer.h"
#include "PathFinding.h"
#include "Controller/PioneerAIController.h"
/*** ���� ������ ��� ���� ���� : End ***/

APioneerController::APioneerController()
{
	// setting mouse
	bShowMouseCursor = true; // ���콺�� ���̰� �մϴ�.
	//DefaultMouseCursor = EMouseCursor::Default; // EMouseCursor::�� ���� ���콺 Ŀ�� ����� ������ �� �ֽ��ϴ�.
	DefaultMouseCursor = EMouseCursor::Crosshairs; // EMouseCursor::�� ���� ���콺 Ŀ�� ����� ������ �� �ֽ��ϴ�.
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

	// EŰ: �ٴڿ� �ִ� ������ �ݱ�
	// GŰ: ���� ���⸦ �ٴڿ� ������


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

	// VR ����
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
	// ���콺 Ŀ�� ����
	{
		if (APioneer* MyPawn = Cast<APioneer>(GetPawn()))
		{
			if (MyPawn->CursorToWorld == nullptr)
				return;

			// ������ �Լ��� �������� ����.
			if (MyPawn->bDying == true)
				return;

			//// Trace to see what is under the mouse cursor
			//// ���콺 Ŀ�� �Ʒ��� ������ �����ϴ��� �����մϴ�.
			//FHitResult HitResult;
			//GetHitResultUnderCursor(ECC_Visibility, false, HitResult); // ���ο��� APlayerController::GetHitResultAtScreenPosition(...)�� ����մϴ�.
			//// ECollisionChannel:: indicating different type of objects for rigid-body collision purposes.

			//// Indicates if this hit was a result of blocking collision
			//if (HitResult.bBlockingHit)
			//{
			//	// We hit something, move there
			//	// Hit.ImpactPoint�� Ray�� �浹�� ��ü�� ǥ�� ������ ��ȯ�մϴ�.
			//	PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, HitResult.ImpactPoint);
			//}
			PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, MyPawn->CursorToWorld->GetComponentLocation());
		}
	}
}

///** Navigate player to the current touch location. */
//void APioneerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	// �޾ƿ� ��ġ�� ��ġ�� ScreenSpaceLocation�� �����մϴ�.
//	FVector2D ScreenSpaceLocation(Location);
//
//	// Trace to see what is under the touch location
//	// ��ġ�� ��ġ �Ʒ��� ������ �����ϴ��� �����մϴ�.
//	FHitResult HitResult;
//	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
//
//	// Indicates if this hit was a result of blocking collision
//	if (HitResult.bBlockingHit)
//	{
//		// We hit something, move there
//		// Hit.ImpactPoint�� Ray�� �浹�� ��ü�� ǥ�� ������ ��ȯ�մϴ�.
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
	//// ���� ��Ʈ�ѷ��� ����ϰ� �ִ� Pawn ��ü�� (APioneer*)�� ��ȯ�Ͽ� �����ɴϴ�.
	//APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	// Value�� ���� ������ ������ �ʿ䰡 ������ ����
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::MoveForward: if (!Pioneer || !GetPawn())"));
		return;
	}
	
	// ������ �Լ��� �������� ����.
	if (Pioneer->bDying)
		return;

	// Navigation���� �̵��ϴ� ���̾����� ����ϴ�. 
	StopMovement();

	//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // ��Ʈ�ѷ��� ȸ������ �����ɴϴ�.
	const FRotator Rotation = Pioneer->CameraBoomRotation; // Pioneer�� ī�޶� ȸ������ �����ɴϴ�.
	const FRotator YawRotation(0, Rotation.Yaw, 0); // ������ ������ ã���ϴ�.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	Pioneer->AddMovementInput(Direction, Value); // �ش� �������� �̵� ���� �߰��մϴ�.

	//// ������ �����մϴ�.
	//MyPawn->AddMovementInput(FVector().ForwardVector, value);
}

void APioneerController::MoveRight(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ ����
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::MoveRight: if (!Pioneer || !GetPawn())"));
		return;
	}


	// ������ �Լ��� �������� ����.
	if (Pioneer->bDying)
		return;

	// Navigation���� �̵��ϴ� ���̾����� ����ϴ�. 
	StopMovement();

	//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // ��Ʈ�ѷ��� ȸ������ �����ɴϴ�.
	const FRotator Rotation = Pioneer->CameraBoomRotation; // Pioneer�� ī�޶� ȸ������ �����ɴϴ�.
	const FRotator YawRotation(0, Rotation.Yaw, 0); // ������ ������ ã���ϴ�.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	Pioneer->AddMovementInput(Direction, Value); // �ش� �������� �̵� ���� �߰��մϴ�.

	//// ������ �����մϴ�.
	//MyPawn->AddMovementInput(FVector().RightVector, value);
}

void APioneerController::ZoomInOrZoomOut(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ ����
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::ZoomInOrZoomOut: if (!Pioneer || !GetPawn())"));
		return;
	}


	// ������ �Լ��� �������� ����.
	if (Pioneer->bDying)
		return;

	Pioneer->ZoomInOrZoomOut(Value);
}
void APioneerController::FireWeapon(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ ����
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::FireWeapon: if (!Pioneer || !GetPawn())"));
		return;
	}


	// ������ �Լ��� �������� ����.
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


	// ������ �Լ��� �������� ����.
	if (Pioneer->bDying)
		return;

	// �Ǽ����� �������� ����.
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


	// ������ �Լ��� �������� ����.
	if (Pioneer->bDying)
		return;

	// �Ǽ����� �������� ����.
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


	// ������ �Լ��� �������� ����.
	if (Pioneer->bDying)
		return;

	if (Pioneer->CurrentWeapon)
		Pioneer->Disarming();
	else
		Pioneer->Arming();
}


void APioneerController::RotatingBuilding(float Value)
{
	// Value�� ���� ������ ������ �ʿ䰡 ������ ����
	if (Value == 0.0f)
		return;

	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::RotatingBuilding: if (!Pioneer || !GetPawn())"));
		return;
	}


	// ������ �Լ��� �������� ����.
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


	// ������ �Լ��� �������� ����.
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
//	// ������ �Լ��� �������� ����.
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