// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerController.h"

APioneerController::APioneerController()
{
	// setting mouse
	bShowMouseCursor = true; // ���콺�� ���̰� �մϴ�.
	DefaultMouseCursor = EMouseCursor::Default; // EMouseCursor::�� ���� ���콺 Ŀ�� ����� ������ �� �ֽ��ϴ�.
	//DefaultMouseCursor = EMouseCursor::Crosshairs; // EMouseCursor::�� ���� ���콺 Ŀ�� ����� ������ �� �ֽ��ϴ�.

	num = 1;
	tempNum = 1;
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
	InputComponent->BindAction("Tab", IE_Pressed, this, &APioneerController::SwitchingPawn);
	InputComponent->BindAction("Temp", IE_Pressed, this, &APioneerController::TempRight);

	
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

		// Trace to see what is under the mouse cursor
		// ���콺 Ŀ�� �Ʒ��� ������ �����ϴ��� �����մϴ�.
		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, HitResult); // ���ο��� APlayerController::GetHitResultAtScreenPosition(...)�� ����մϴ�.
		// ECollisionChannel:: indicating different type of objects for rigid-body collision purposes.

		// Indicates if this hit was a result of blocking collision
		if (HitResult.bBlockingHit)
		{
			// We hit something, move there
			// Hit.ImpactPoint�� Ray�� �浹�� ��ü�� ǥ�� ������ ��ȯ�մϴ�.
			PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, HitResult.ImpactPoint);
		}
	}
}

/** Navigate player to the current touch location. */
void APioneerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// �޾ƿ� ��ġ�� ��ġ�� ScreenSpaceLocation�� �����մϴ�.
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	// ��ġ�� ��ġ �Ʒ��� ������ �����ϴ��� �����մϴ�.
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);

	// Indicates if this hit was a result of blocking collision
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		// Hit.ImpactPoint�� Ray�� �浹�� ��ü�� ǥ�� ������ ��ȯ�մϴ�.
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


void APioneerController::MoveForward(float value)
{
	// ���� ��Ʈ�ѷ��� ����ϰ� �ִ� Pawn ��ü�� (APioneer*)�� ��ȯ�Ͽ� �����ɴϴ�.
	APioneer* const MyPawn = (APioneer*)GetPawn();

	if (MyPawn && (value != 0.0f))
	{
		//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // ��Ʈ�ѷ��� ȸ������ �����ɴϴ�.
		const FRotator Rotation = MyPawn->CameraBoomRotation; // Pioneer�� ī�޶� ȸ������ �����ɴϴ�.
		const FRotator YawRotation(0, Rotation.Yaw, 0); // ������ ������ ã���ϴ�.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // ������ ���͸� ���մϴ�.
		MyPawn->AddMovementInput(Direction, value); // �ش� �������� �̵� ���� �߰��մϴ�.

		//// ������ �����մϴ�.
		//MyPawn->AddMovementInput(FVector().ForwardVector, value);
	}
}

void APioneerController::MoveRight(float value)
{
	// ���� ��Ʈ�ѷ��� ����ϰ� �ִ� Pawn ��ü�� (APioneer*)�� ��ȯ�Ͽ� �����ɴϴ�.
	APioneer* const MyPawn = (APioneer*)GetPawn();

	if (MyPawn && (value != 0.0f))
	{
		//const FRotator Rotation = GetPawn()->Controller->GetControlRotation(); // ��Ʈ�ѷ��� ȸ������ �����ɴϴ�.
		const FRotator Rotation = MyPawn->CameraBoomRotation; // Pioneer�� ī�޶� ȸ������ �����ɴϴ�.
		const FRotator YawRotation(0, Rotation.Yaw, 0); // ������ ������ ã���ϴ�.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // ������ ���͸� ���մϴ�.
		MyPawn->AddMovementInput(Direction, value); // �ش� �������� �̵� ���� �߰��մϴ�.

		//// ������ �����մϴ�.
		//MyPawn->AddMovementInput(FVector().RightVector, value);
	}
}

void APioneerController::SwitchingPawn()
{
	AWorldViewCameraActor* worldViewCameraActor = nullptr;

	for (TActorIterator<AWorldViewCameraActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		worldViewCameraActor = *ActorItr;
	}

	for (TActorIterator<APioneerManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		APioneerManager* pioneerManager = *ActorItr;

		num++;
		if (num == 3)
		{
			num = 1;
		}

		//enum EViewTargetBlendFunction
		//{
		//	/** Camera does a simple linear interpolation. */
		//	VTBlend_Linear,
		//	/** Camera has a slight ease in and ease out, but amount of ease cannot be tweaked. */
		//	VTBlend_Cubic,
		//	/** Camera immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by BlendExp. */
		//	VTBlend_EaseIn,
		//	/** Camera smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by BlendExp. */
		//	VTBlend_EaseOut,
		//	/** Camera smoothly accelerates and decelerates.  Ease amount controlled by BlendExp. */
		//	VTBlend_EaseInOut,
		//	VTBlend_MAX,
		//};
		switch (tempNum)
		{
		case 1:
			SetViewTargetWithBlend(worldViewCameraActor, 1.0f, EViewTargetBlendFunction::VTBlend_Linear, 10.0f, true);
			break;
		case 2:
			SetViewTargetWithBlend(worldViewCameraActor, 1.0f, EViewTargetBlendFunction::VTBlend_Cubic, 10.0f, true);
			break;
		case 3:
			SetViewTargetWithBlend(worldViewCameraActor, 1.0f, EViewTargetBlendFunction::VTBlend_EaseIn, 10.0f, true);
			break;
		case 4:
			SetViewTargetWithBlend(worldViewCameraActor, 1.0f, EViewTargetBlendFunction::VTBlend_EaseOut, 10.0f, true);
			break;
		case 5:
			SetViewTargetWithBlend(worldViewCameraActor, 1.0f, EViewTargetBlendFunction::VTBlend_EaseInOut, 10.0f, true);
			break;
		case 6:
			SetViewTargetWithBlend(worldViewCameraActor, 1.0f, EViewTargetBlendFunction::VTBlend_MAX, 10.0f, true);
			break;
		}

		FTimerHandle Timer;
		GetWorldTimerManager().SetTimer(Timer, this, &APioneerController::SwitchingCamera, 1.0f, false);
		FTimerHandle Timer2;
		GetWorldTimerManager().SetTimer(Timer2, this, &APioneerController::PossessPioneer, 2.0f, false);
		
	}
}

void APioneerController::SwitchingCamera()
{
	for (TActorIterator<APioneerManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		APioneerManager* pioneerManager = *ActorItr;

		switch (tempNum)
		{
		case 1:
			SetViewTargetWithBlend(pioneerManager->TmapPioneers[num], 1.0f, EViewTargetBlendFunction::VTBlend_Linear, 10.0f, true);
			break;
		case 2:
			SetViewTargetWithBlend(pioneerManager->TmapPioneers[num], 1.0f, EViewTargetBlendFunction::VTBlend_Cubic, 10.0f, true);
			break;
		case 3:
			SetViewTargetWithBlend(pioneerManager->TmapPioneers[num], 1.0f, EViewTargetBlendFunction::VTBlend_EaseIn, 10.0f, true);
			break;
		case 4:
			SetViewTargetWithBlend(pioneerManager->TmapPioneers[num], 1.0f, EViewTargetBlendFunction::VTBlend_EaseOut, 10.0f, true);
			break;
		case 5:
			SetViewTargetWithBlend(pioneerManager->TmapPioneers[num], 1.0f, EViewTargetBlendFunction::VTBlend_EaseInOut, 10.0f, true);
			break;
		case 6:
			SetViewTargetWithBlend(pioneerManager->TmapPioneers[num], 1.0f, EViewTargetBlendFunction::VTBlend_MAX, 10.0f, true);
			break;
		}
	}
}

void APioneerController::PossessPioneer()
{
	for (TActorIterator<APioneerManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APioneerManager* pioneerManager = *ActorItr;

		if (GetPawn())
			UnPossess();

		if (pioneerManager->TmapPioneers.Contains(num))
			Possess(pioneerManager->TmapPioneers[num]);
	}
}

void APioneerController::TempRight()
{
	tempNum++;
	if (tempNum == 7)
		tempNum = 1;

}