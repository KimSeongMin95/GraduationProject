// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerController.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "PathFinding.h"

#include "Character/Pioneer.h"
#include "Controller/PioneerAIController.h"

#include "Item/Item.h"
#include "Item/Weapon/Weapon.h"

#include "GameMode/TutorialGameMode.h"
#include "GameMode/OnlineGameMode.h"

#include "PioneerManager.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
APioneerController::APioneerController()
{
	Pioneer = nullptr;

	bScoreBoard = false;

	PioneerManager = nullptr;

	bObservation = true;

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
	// FŰ: �ٴڿ� �ִ� ������ �ݱ�
	InputComponent->BindAction("AcquireItem", IE_Pressed, this, &APioneerController::AcquireItem);
	// GŰ: ���� ���⸦ �ٴڿ� ������
	InputComponent->BindAction("AbandonWeapon", IE_Pressed, this, &APioneerController::AbandonWeapon);


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
	// TabŰ: ���ھ� ����
	InputComponent->BindAxis("ScoreBoard", this, &APioneerController::ScoreBoard);

	// Left ArrowŰ: ���� ����
	InputComponent->BindAction("Observation_Left", IE_Pressed, this, &APioneerController::ObservationLeft);
	// Right ArrowŰ: ���� ����
	InputComponent->BindAction("Observation_Right", IE_Pressed, this, &APioneerController::ObservationRight);
	// Space BarŰ: ���� ��������
	InputComponent->BindAction("Observation_Free", IE_Pressed, this, &APioneerController::ObservationFree);
	// EnterŰ: ���� �������� AI Pioneer�� ����
	InputComponent->BindAction("Observation_Possess", IE_Pressed, this, &APioneerController::ObservationPossess);
}
/*** Basic Function : End ***/


/*** APlayerController : Start ***/
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

		bObservation = false;
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

	bObservation = true;
}
/*** APlayerController : End ***/


/*** APioneerController : Start ***/
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
		if (APioneer* pioneer = Cast<APioneer>(GetPawn()))
		{
			if (pioneer->GetCursorToWorld() == nullptr)
				return;

			// ������ �Լ��� �������� ����.
			if (pioneer->bDying == true)
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
			PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, pioneer->GetCursorToWorld()->GetComponentLocation());
		}
	}
}

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

	// ���ھ������ ��Ÿ�������� �������� �ʽ��ϴ�.
	if (bScoreBoard)
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

	if (Pioneer->GetCurrentWeapon())
		Pioneer->Disarming();
	else
		Pioneer->Arming();
}



void APioneerController::AcquireItem()
{
	UE_LOG(LogTemp, Warning, TEXT("APioneerController::AcquireItem()"));


	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::ChangeWeapon: if (!Pioneer || !GetPawn())"));
		return;
	}

	if (Pioneer->OverlapedItems.Num() <= 0)
		return;
	
	AItem* closestItem = nullptr;
	float minDistance = 1000000.0f;
	for (auto& item : Pioneer->OverlapedItems)
	{
		if (!item)
			continue;

		float dist = FVector::Distance(Pioneer->GetActorLocation(), item->GetActorLocation());
		if (dist < minDistance)
		{
			minDistance = dist;
			closestItem = item;
		}
	}

	if (!closestItem)
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("APioneerController::AcquireItem(): find"));

	if (closestItem->IsA(AWeapon::StaticClass()))
		Pioneer->AcquireWeapon(Cast<AWeapon>(closestItem));
}

void APioneerController::AbandonWeapon()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::ChangeWeapon: if (!Pioneer || !GetPawn())"));
		return;
	}

	Pioneer->AbandonWeapon();
}

void APioneerController::ConstructingMode()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::PlaceBuilding: if (!Pioneer || !GetPawn())"));
		return;
	}

	// ������ �Լ��� �������� ����.
	if (Pioneer->bDying)
		return;

	Pioneer->Disarming();

	Pioneer->bConstructingMode = true;
}
void APioneerController::ESC_ConstructingMode()
{
	if (!Pioneer || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerController::PlaceBuilding: if (!Pioneer || !GetPawn())"));
		return;
	}

	// ������ �Լ��� �������� ����.
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

	if (Pioneer->bConstructingMode)
		Pioneer->SpawnBuilding((int)Value);
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

void APioneerController::Menu()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <APioneerController::Menu()> if (!world)\n");
		return;
	}

	ATutorialGameMode* tutorialGameMode = Cast<ATutorialGameMode>(UGameplayStatics::GetGameMode(world));
	AOnlineGameMode* onlineGameMode = Cast<AOnlineGameMode>(UGameplayStatics::GetGameMode(world));

	if (tutorialGameMode)
	{

	}
	else if (onlineGameMode)
	{
		onlineGameMode->ToggleInGameMenuWidget();
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
				printf_s("[ERROR] <APioneerController::ScoreBoardPressed()> if (!world)\n");
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
		printf_s("[ERROR] <APioneerController::ScoreBoardPressed()> if (!world)\n");
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

void APioneerController::ObservationLeft()
{
	if (!PioneerManager)
	{
		printf_s("[ERROR] <APioneerController::ObservationLeft()> if (!PioneerManager)\n");
		return;
	}


}
void APioneerController::ObservationRight()
{
	if (!PioneerManager)
	{
		printf_s("[ERROR] <APioneerController::ObservationLeft()> if (!PioneerManager)\n");
		return;
	}


}
void APioneerController::ObservationFree()
{
	if (!PioneerManager)
	{
		printf_s("[ERROR] <APioneerController::ObservationLeft()> if (!PioneerManager)\n");
		return;
	}


}
void APioneerController::ObservationPossess()
{
	if (!PioneerManager)
	{
		printf_s("[ERROR] <APioneerController::ObservationLeft()> if (!PioneerManager)\n");
		return;
	}


}



void APioneerController::SetPioneerManager(class APioneerManager* PM)
{
	this->PioneerManager = PM;
}
/*** APioneerController : End ***/



//void APioneerController::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}