// Fill out your copyright notice in the Description page of Project Settings.


#include "PioneerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h" // VR
#include "Pioneer.h"
#include "Engine/World.h"

APioneerController::APioneerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void APioneerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	//APlayerController::PlayerTick(DeltaTime);

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
	//APlayerController::SetupInputComponent();

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
}

//void APioneerController::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}

/** Navigate player to the current mouse cursor location. */
void APioneerController::MoveToMouseCursor()
{
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
	{
		
		// Trace to see what is under the mouse cursor
		// 마우스 커서 아래에 무엇이 존재하는지 추적합니다.
		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, HitResult); // 내부에서 APlayerController::GetHitResultAtScreenPosition(...)을 사용합니다.
		// ECollisionChannel:: indicating different type of objects for rigid-body collision purposes.

		// Indicates if this hit was a result of blocking collision
		if (HitResult.bBlockingHit)
		{
			// We hit something, move there
			// Hit.ImpactPoint는 Ray와 충돌한 물체의 표면 지점을 반환합니다.
			SetNewMoveDestination(HitResult.ImpactPoint);
		}
	}
}

/** Navigate player to the current touch location. */
void APioneerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
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
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

/** Navigate player to the given world location. */
void APioneerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();

	if (MyPawn)
	{
		// 현재 Pawn의 위치와 목적지 간의 거리를 구합니다.
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		// 걷기 애니메이션이 정확하게 플레이될 수 있도록 어느 정도 거리가 있어야 움직일 수 있게 합니다.
		if ((Distance > 120.0f))
		{
			// UAIBlueprintHelperLibrary의 함수를 사용하여 움직입니다.
			//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
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

void APioneerController::MoveForward(float value)
{
	if ((GetPawn()->Controller != NULL) && (value != 0.0f))
	{
		// 전방 방향을 찾습니다.
		const FRotator Rotation = GetPawn()->Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 전방 벡터를 구합니다.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// 해당 방향으로 이동 값을 추가합니다.
		GetPawn()->AddMovementInput(Direction, value);
	}
}

void APioneerController::MoveRight(float value)
{
	if ((GetPawn()->Controller != NULL) && (value != 0.0f))
	{
		// 오른쪽 방향을 찾습니다.
		const FRotator Rotation = GetPawn()->Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 오른쪽 벡터를 구합니다.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 해당 방향으로 이동 값을 추가합니다.
		GetPawn()->AddMovementInput(Direction, value);
	}
}