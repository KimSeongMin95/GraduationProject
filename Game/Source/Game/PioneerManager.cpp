// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Etc/WorldViewCameraActor.h"
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"

#include "Network/ClientSocket.h"
#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "CustomWidget/InGameWidget.h"

#include "Item/Weapon/Weapon.h"

#include "BuildingManager.h"
/*** ���� ������ ��� ���� ���� : End ***/

class cInfoOfResources APioneerManager::Resources;

/*** Basic Function : Start ***/
APioneerManager::APioneerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	//SwitchState = ESwitchState::Switchable;

	ViewTarget = nullptr;

	ClientSocket = nullptr;
	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;

	ViewpointState = EViewpointState::Idle;

	// Pioneer�� Default�� 0�̹Ƿ� �� 1���� �����ؾ� �մϴ�.
	KeyID = 1;

	IdCurrentlyBeingObserved = 0;

	Resources = cInfoOfResources();

	SceneCapture2D = nullptr;

	BuildingManager = nullptr;
}

void APioneerManager::BeginPlay()
{
	Super::BeginPlay();

	// ī�޶���� �����մϴ�.
	SpawnWorldViewCameraActor(&FreeViewCamera, FTransform(FRotator(-30.0f, 45.0f, 0.0f), FVector(-9969.8f, -9484.5f, 1441.6f)));

	SpawnWorldViewCameraActor(&CameraOfCurrentPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));
	SpawnWorldViewCameraActor(&WorldViewCameraOfCurrentPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));
	SpawnWorldViewCameraActor(&WorldViewCameraOfNextPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));
	SpawnWorldViewCameraActor(&CameraOfNextPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));


	//////////////////////////
	// ��Ʈ��ũ
	//////////////////////////
	ClientSocket = cClientSocket::GetSingleton();
	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();


	FindPioneersInWorld();

	FindSceneCapture2D();
}

void APioneerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickOfObservation();

	TickOfViewTarget();

	TickOfResources();


	//SwitchTick();
}
/*** Basic Function : End ***/


/*** APioneerManager : Start ***/
void APioneerManager::SpawnWorldViewCameraActor(class AWorldViewCameraActor** WorldViewCameraActor, FTransform Transform)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::SpawnWorldViewCameraActor(...)> if (!world)"));
#endif
		return;
	}

	FTransform myTrans = Transform;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name�� �����մϴ�. World Outliner�� ǥ��Ǵ� Label���� �ٸ��ϴ�.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	*WorldViewCameraActor = world->SpawnActor<AWorldViewCameraActor>(AWorldViewCameraActor::StaticClass(), myTrans, SpawnParams);

	// ��Ⱦ�� ������ �����ν� ȭ���� ������ ������ �����մϴ�.
	if (*WorldViewCameraActor)
		(*WorldViewCameraActor)->GetCameraComponent()->bConstrainAspectRatio = false;
}

void APioneerManager::FindPioneersInWorld()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::FindPioneersInWorld()> if (!world)"));
#endif
		return;
	}

	for (TActorIterator<APioneer> ActorItr(world); ActorItr; ++ActorItr)
	{
		ActorItr->SetPioneerManager(this);

		ActorItr->SetBuildingManager(BuildingManager);

		// �̹� �߰��Ǿ����� �ʴٸ�
		if (Pioneers.Contains(KeyID) == false)
		{
			Pioneers.Add(KeyID, *ActorItr);
			ActorItr->ID = KeyID;

			if (ServerSocketInGame)
			{
				// �̹� ������ Pioneer�� ���Ӽ����� �˸��ϴ�.
				if (ServerSocketInGame->IsServerOn())
				{
					cInfoOfPioneer infoOfPioneer = ActorItr->GetInfoOfPioneer();
					ServerSocketInGame->SendSpawnPioneer(infoOfPioneer);
				}
			}

			KeyID++;
		}
	}
}

void APioneerManager::FindSceneCapture2D()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::FindSceneCapture2D()> if (!world)"));
#endif
		return;
	}

	for (TActorIterator<ASceneCapture2D> ActorItr(world); ActorItr; ++ActorItr)
	{
		SceneCapture2D = *ActorItr;
	}
}


///////////////////////////////////////////
//// ViewTarget�� Possess ��ȯ
///////////////////////////////////////////
//void APioneerManager::SwitchViewTarget(AActor* Actor, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
//{
//	if (!Actor)
//	{
//		printf_s("[ERROR] <APioneerManager::SwitchViewTarget(...)> if (!Actor)\n");
//		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchViewTarget(...)> if (!Actor)"));
//		return;
//	}
//	
//	if (!PioneerController)
//	{
//		printf_s("[ERROR] <APioneerManager::SwitchViewTarget(...)> if (!PioneerController)\n");
//		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchViewTarget(...)> if (!PioneerController)"));
//		return;
//	}
//	
//	PioneerController->SetViewTargetWithBlend(Actor, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//	printf_s("[INFO] <APioneerManager::SwitchViewTarget(...)> %s, BlendTime: %f\n", TCHAR_TO_ANSI(*Actor->GetName()), BlendTime);
//	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchViewTarget(...)> %s, BlendTime: %f"), *Actor->GetName(), BlendTime);
//}
//
//void APioneerManager::FindTargetViewActor(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
//{
//	printf_s("[INFO] <APioneerManager::FindTargetViewActor(...)>\n");
//	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::FindTargetViewActor(...)>"));
//	SwitchState = ESwitchState::FindTargetViewActor;
//
//	TargetViewActor = nullptr;
//
//	//for (auto& pioneer : Pioneers)
//	//{
//	//	if (!pioneer)
//	//		continue;
//
//	//	if (pioneer->bDying || pioneer->IsActorBeingDestroyed())
//	//		continue;
//
//	//	if (pioneer->SocketID == -1)
//	//		TargetViewActor = pioneer;
//	//}
//
//
//	// ��ȯ�� APioneer�� ã����
//	if (TargetViewActor)
//	{
//		printf_s("[INFO] <APioneerManager::FindTargetViewActor(...)> if (TargetViewActor)\n");
//		UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::FindTargetViewActor(...)> if (TargetViewActor)"));
//
//		FVector location = TargetViewActor->GetActorLocation();
//		location.Z = 5000.0f;
//
//		if (WorldViewCameraOfNextPioneer)
//		{
//			WorldViewCameraOfNextPioneer->SetActorLocation(location);
//			SwitchViewTarget(WorldViewCameraOfNextPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//		}
//		else
//		{
//			printf_s("[ERROR] <APioneerManager::FindTargetViewActor(...)> if (!WorldViewCameraOfNextPioneer)\n");
//			UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::FindTargetViewActor(...)> if (!WorldViewCameraOfNextPioneer)"));
//		}
//
//		GetWorldTimerManager().ClearTimer(TimerHandleOfFindTargetViewActor);
//
//		FTimerDelegate timerDel;
//		timerDel.BindUFunction(this, FName("SwitchNext"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
//		GetWorldTimerManager().SetTimer(TimerOfSwitchNext, timerDel, 0.1f, false, BlendTime + 0.5f);
//	}
//}
//
//void APioneerManager::SwitchNext(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
//{
//	printf_s("[INFO] <APioneerManager::SwitchNext(...)>\n");
//	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchNext(...)>"));
//
//	SwitchState = ESwitchState::Next;
//
//	if (!TargetViewActor)
//	{
//		SwitchState = ESwitchState::Switchable; 
//
//		printf_s("[ERROR] <APioneerManager::SwitchNext(...)> if (!TargetViewActor)\n");
//		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchNext(...)> if (!TargetViewActor)"));
//		return;
//	}
//
//	if (APioneer* pioneer = Cast<APioneer>(TargetViewActor))
//	{
//		if (pioneer->bDying || pioneer->IsActorBeingDestroyed())
//		{
//			printf_s("[INFO] <APioneerManager::SwitchNext(...)> if (pioneer->bDying || pioneer->IsActorBeingDestroyed())\n");
//			UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchNext(...)> if (pioneer->bDying || pioneer->IsActorBeingDestroyed())"));
//
//			// FindTargetViewActor�� WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer ��ȯ ������ �ذ��ϱ� ����,
//			// WorldViewCameraOfNextPioneer�� ��ġ�� WorldViewCameraOfCurrentPioneer�� �����ϰ� ���� WorldViewCameraOfCurrentPioneer�� ��ȯ
//			if (WorldViewCameraOfCurrentPioneer && WorldViewCameraOfNextPioneer)
//			{
//				WorldViewCameraOfCurrentPioneer->SetActorTransform(WorldViewCameraOfNextPioneer->GetActorTransform());
//
//				if (PioneerController)
//					PioneerController->SetViewTarget(WorldViewCameraOfCurrentPioneer);
//				else
//				{
//					printf_s("[ERROR] <APioneerManager::SwitchNext(...)> if (WorldViewCameraOfCurrentPioneer && WorldViewCameraOfNextPioneer) && if (!PioneerController)\n");
//					UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchNext(...)> if (WorldViewCameraOfCurrentPioneer && WorldViewCameraOfNextPioneer) && if (!PioneerController)"));
//				}
//			}
//			else
//			{
//				printf_s("[ERROR] <APioneerManager::SwitchNext(...)> if (!WorldViewCameraOfCurrentPioneer || !WorldViewCameraOfNextPioneer)\n");
//				UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchNext(...)> if (!WorldViewCameraOfCurrentPioneer || !WorldViewCameraOfNextPioneer)"));
//			}
//
//			FTimerDelegate timerDel;
//			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
//			GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true);
//
//			return;
//		}
//
//		pioneer->CopyTopDownCameraTo(CameraOfCurrentPioneer);
//	}
//
//	if (!PioneerController)
//	{
//		printf_s("[ERROR] <APioneerManager::SwitchNext(...)> if (!PioneerController)\n");
//		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchNext(...)> if (!PioneerController)"));
//		return;
//	}
//
//	PioneerController->SetViewTargetWithBlend(CameraOfNextPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//	printf_s("[INFO] <APioneerManager::SwitchNext(...)> %s, BlendTime: %f\n", TCHAR_TO_ANSI(*CameraOfNextPioneer->GetName()), BlendTime);
//	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchNext(...)> %s, BlendTime: %f"), *CameraOfNextPioneer->GetName(), BlendTime);
//
//	FTimerDelegate timerDel;
//	timerDel.BindUFunction(this, FName("SwitchFinish"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
//	GetWorldTimerManager().SetTimer(TimerHandleOfSwitchFinish, timerDel, 0.1f, false, BlendTime + 0.25f);
//}
//
//void APioneerManager::SwitchFinish(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
//{
//	printf_s("[INFO] <APioneerManager::SwitchFinish(...)>\n");
//	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchFinish(...)>"));
//
//	SwitchState = ESwitchState::Finish;
//
//	if (!TargetViewActor)
//	{
//		SwitchState = ESwitchState::Switchable;
//
//		printf_s("[ERROR] <APioneerManager::SwitchFinish(...)> if (!TargetViewActor)\n");
//		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchFinish(...)> if (!TargetViewActor)"));
//		return;
//	}
//	
//	if (APioneer* pioneer = Cast<APioneer>(TargetViewActor))
//	{
//		if (pioneer->bDying || pioneer->IsActorBeingDestroyed())
//		{
//			// FindTargetViewActor�� WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer ��ȯ ������ �ذ��ϱ� ����,
//			// WorldViewCameraOfNextPioneer�� ��ġ�� WorldViewCameraOfCurrentPioneer�� �����ϰ� ���� WorldViewCameraOfCurrentPioneer�� ��ȯ
//			if (WorldViewCameraOfCurrentPioneer && WorldViewCameraOfNextPioneer)
//			{
//				WorldViewCameraOfCurrentPioneer->SetActorTransform(WorldViewCameraOfNextPioneer->GetActorTransform());
//				SwitchViewTarget(WorldViewCameraOfCurrentPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//			}
//			else
//			{
//				printf_s("[ERROR] <APioneerManager::SwitchFinish(...)> if (!WorldViewCameraOfCurrentPioneer || !WorldViewCameraOfNextPioneer)\n");
//				UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchFinish(...)> if (!WorldViewCameraOfCurrentPioneer || !WorldViewCameraOfNextPioneer)"));
//			}
//			printf_s("[INFO] <APioneerManager::SwitchFinish(...)> if (pioneer->bDying || pioneer->IsActorBeingDestroyed())\n");
//			UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchFinish(...)> if (pioneer->bDying || pioneer->IsActorBeingDestroyed())"));
//
//			FTimerDelegate timerDel;
//			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
//			GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
//
//			return;
//		}
//
//		PossessPioneer(Cast<APioneer>(TargetViewActor));
//		SwitchState = ESwitchState::Switchable;
//	}
//
//}
//
//void APioneerManager::PossessPioneer(class APioneer* Pioneer)
//{
//	printf_s("[INFO] <APioneerManager::PossessPioneer(...)>\n");
//	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::PossessPioneer(...)>"));
//
//	// �÷��̾ �����ϴ� ��ô�ڸ� �����մϴ�.
//	PioneerOfPlayer = Cast<APioneer>(TargetViewActor);
//
//	// TargetViewActor�� �ٽ� �ʱ�ȭ�մϴ�.
//	TargetViewActor = nullptr;
//
//	// PioneerController�� �����ϴ��� Ȯ���մϴ�.
//	if (!PioneerController)
//	{
//		printf_s("[ERROR] <APioneerManager::PossessPioneer(...)> if (!PioneerController)\n");
//		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::PossessPioneer(...)> if (!PioneerController)"));
//		return;
//	}
//
//	//// PioneerController�� Pawn�� �����ϰ� ������ �Ҹ��ŵ�ϴ�.
//	//if (PioneerController->GetPawn())
//	//	PioneerController->GetPawn()->Destroy();
//
//	PioneerController->Possess(Pioneer);
//
//	printf_s("[INFO] <APioneerManager::PossessPioneer(...)> PioneerController->Possess(Pioneer);\n");
//	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::PossessPioneer(...)> PioneerController->Possess(Pioneer);"));
//}
//
//void APioneerManager::SwitchTick()
//{
//	if (SwitchState == ESwitchState::Next || SwitchState == ESwitchState::Finish)
//	{
//		if (TargetViewActor)
//		{
//			// ��ġ ����
//			if (TargetViewActor->IsA(APioneer::StaticClass()))
//			{
//				Cast<APioneer>(TargetViewActor)->CopyTopDownCameraTo(CameraOfNextPioneer);
//			}
//		}
//	}
//}


/////////////////////////////////////////
// public
/////////////////////////////////////////
void APioneerManager::SetPioneerController(class APioneerController* pPioneerController)
{
	this->PioneerController = pPioneerController;
}

void APioneerManager::SetBuildingManager(class ABuildingManager* pBuildingManager)
{
	this->BuildingManager = pBuildingManager;
}

void APioneerManager::SetInGameWidget(class UInGameWidget* pInGameWidget)
{
	this->InGameWidget = pInGameWidget;
}

void APioneerManager::SpawnPioneer(FTransform Transform)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::SpawnPioneer(...)> if (!world)"));
#endif
		return;
	}

	FTransform myTrans = Transform;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name�� �����մϴ�. World Outliner�� ǥ��Ǵ� Label���� �ٸ��ϴ�.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	/* SpawnParams.Instigator = Instigator;
	�𸮾� ������ ���� �����ӿ��� ��� ���Ϳ��� ������(Instigator)��� ������ �����Ǿ� �ֽ��ϴ�.
	�̴� ������ �������� ���� ������ ������ ���� �뵵�� ���Ǵµ�,
	�ݵ�� �������� ���� ���͸� ���������� �ʽ��ϴ�.
	���� �� �ڽ��� ������ ���Ͷ����, Ž���� �� �� �ֿ� ����� �����ϴµ� �����ϰ� ����� �� �ֽ��ϴ�.
	*/
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.
	///** Fall back to default settings. */
	//Undefined								UMETA(DisplayName = "Default"),
	///** Actor will spawn in desired location, regardless of collisions. */
	//AlwaysSpawn								UMETA(DisplayName = "Always Spawn, Ignore Collisions"),
	///** Actor will try to find a nearby non-colliding location (based on shape components), but will always spawn even if one cannot be found. */
	//AdjustIfPossibleButAlwaysSpawn			UMETA(DisplayName = "Try To Adjust Location, But Always Spawn"),
	///** Actor will try to find a nearby non-colliding location (based on shape components), but will NOT spawn unless one is found. */
	//AdjustIfPossibleButDontSpawnIfColliding	UMETA(DisplayName = "Try To Adjust Location, Don't Spawn If Still Colliding"),
	///** Actor will fail to spawn. */
	//DontSpawnIfColliding					UMETA(DisplayName = "Do Not Spawn"),

	APioneer* pioneer = world->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);

	pioneer->SetPioneerManager(this);
	
	pioneer->SetBuildingManager(BuildingManager);

	// �̹� �߰��Ǿ����� �ʴٸ�
	if (Pioneers.Contains(KeyID) == false)
	{
		pioneer->ID = KeyID;
		Pioneers.Add(KeyID, pioneer);
	}

	if (ServerSocketInGame)
	{
		// Pioneer ������ ����Ŭ���̾�Ʈ�鿡�� �˸��ϴ�.
		if (ServerSocketInGame->IsServerOn())
		{
			cInfoOfPioneer infoOfPioneer = pioneer->GetInfoOfPioneer();
			ServerSocketInGame->SendSpawnPioneer(infoOfPioneer);
		}
	}

	KeyID++;


	if (ViewpointState == EViewpointState::SpaceShip)
	{
		Observation();
	}
}

void APioneerManager::SpawnPioneerByRecv(class cInfoOfPioneer& InfoOfPioneer)
{
	// �̹� �����ϸ� �������� �ʰ� ���� �����մϴ�.
	if (Pioneers.Contains(InfoOfPioneer.ID))
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneerManager::SpawnPioneerByRecv(...)> if (Pioneers.Contains(InfoOfPioneer.ID))"));
#endif
		Pioneers[InfoOfPioneer.ID]->SetInfoOfPioneer(InfoOfPioneer);
		return;
	}

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::SpawnPioneerByRecv(...)> if (!world)"));
#endif
		return;
	}

	/*******************************************************************/

	FTransform myTrans = InfoOfPioneer.Animation.GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator; 

	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	APioneer* pioneer = world->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);

	// �浹���� ��ġ�� ����
	while (!pioneer)
	{
		FVector location = myTrans.GetLocation();
		location.X += 100.0f;
		myTrans.SetLocation(location);

		pioneer = world->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);
	}

	pioneer->SetPioneerManager(this);

	pioneer->ID = InfoOfPioneer.ID;
	pioneer->SetInfoOfPioneer(InfoOfPioneer);
	Pioneers.Add(InfoOfPioneer.ID, pioneer);
	

	if (ViewpointState == EViewpointState::SpaceShip)
	{
		Observation();
	}
}

//void APioneerManager::SwitchOtherPioneer(class APioneer* CurrentPioneer, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
//{
//	//enum EViewTargetBlendFunction
//	//{
//	//	/** Camera does a simple linear interpolation. */
//	//	VTBlend_Linear,
//	//	/** Camera has a slight ease in and ease out, but amount of ease cannot be tweaked. */
//	//	VTBlend_Cubic,
//	//	/** Camera immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by BlendExp. */
//	//	VTBlend_EaseIn,
//	//	/** Camera smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by BlendExp. */
//	//	VTBlend_EaseOut,
//	//	/** Camera smoothly accelerates and decelerates.  Ease amount controlled by BlendExp. */
//	//	VTBlend_EaseInOut,
//	//	VTBlend_MAX,
//	//};
//
//	/* SwitchPawn ����
//	1. ��� CurrentPioneer�� ī�޶󿡼� CameraOfCurrentPioneer�� ��ȯ
//	2. SwitchTime ���� CameraOfCurrentPioneer���� WorldViewCameraOfCurrentPioneer�� ��ȯ
//	3. ��ȯ��Ű�⿡ ������ Pioneer Ž��
//	4. SwitchTime ���� WorldViewCameraOfCurrentPioneer���� WorldViewCameraOfNextPioneer�� ��ȯ
//	5. Ž���� Pioneer�� ����ִٸ� SwitchTime ���� WorldViewCameraOfNextPioneer���� CameraOfNextPioneer�� ��ȯ
//	6. ��ȯ���� Pioneer�� ������ �ٽ� 3������ ȸ��
//	7.
//	*/
//
//	if (SwitchState != ESwitchState::Switchable)
//	{
//		printf_s("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (SwitchState != ESwitchState::Switchable)\n");
//		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (SwitchState != ESwitchState::Switchable)"));
//		return;
//	}
//
//	if (!PioneerController)
//	{
//		printf_s("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (!PioneerController)\n");
//		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (!PioneerController)"));
//		return;
//	}
//
//	if (CurrentPioneer)
//	{
//		if (CurrentPioneer->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)
//			return;
//
//		PioneerController->SetViewTargetWithBlend(CameraOfCurrentPioneer);
//
//		FVector location = CurrentPioneer->GetActorLocation();
//		location.Z = 5000.0f;
//
//		if (WorldViewCameraOfCurrentPioneer)
//		{
//			WorldViewCameraOfCurrentPioneer->SetActorLocation(location);
//			SwitchViewTarget(WorldViewCameraOfCurrentPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//		}
//		else
//		{
//			printf_s("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (!WorldViewCameraOfCurrentPioneer)\n");
//			UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (!WorldViewCameraOfCurrentPioneer)"));
//		}
//
//		SwitchState = ESwitchState::Current;
//
//		FTimerDelegate timerDel;
//		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
//		GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
//	}
//	else
//	{
//		printf_s("[INFO] <APioneerManager::SwitchOtherPioneer(...)> else\n");
//		UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchOtherPioneer(...)> else"));
//
//		FTimerDelegate timerDel;
//		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
//		GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
//	}
//}


void APioneerManager::TickOfObservation()
{
	// ������ Pioneer�� ������ �������� �ʽ��ϴ�.
	if (PioneerOfPlayer)
	{
		return;
	}

	if (!ViewTarget)
	{
		return;
	}
	if (!PioneerController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfObservation(...)> if (!PioneerController)"));
#endif
		return;
	}
	if (ViewTarget->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfObservation(...)> if (ViewTarget->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)"));
#endif		
		return;
	}
	/***********************************************************************/


	if (WorldViewCameraOfCurrentPioneer)
	{
		FVector location = ViewTarget->GetActorLocation();
		location.Z = 2000.0f;

		WorldViewCameraOfCurrentPioneer->SetActorLocation(location);
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfObservation(...)> if (!WorldViewCameraOfCurrentPioneer)"));
#endif	
	}

}

void APioneerManager::Observation()
{
	if (!PioneerController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::Observation(...)> if (!PioneerController)"));
#endif	
		return;
	}
	/***********************************************************************/

	TArray<int32> keys;
	Pioneers.GetKeys(keys);

	// Pioneer�� 0���̶�� ������������ ��ȯ�մϴ�.
	if (keys.Num() == 0)
	{
		SwitchToFreeViewpoint();
		return;
	}

	// UI ����
	if (InGameWidget)
	{
		InGameWidget->SetArrowButtonsVisibility(true);
		InGameWidget->SetPossessButtonVisibility(true);
		InGameWidget->SetFreeViewpointButtonVisibility(true);
		InGameWidget->SetObservingButtonVisibility(false);

		InGameWidget->SetBuildingBoxVisibility(false);
	}

	if (Pioneers.Contains(IdCurrentlyBeingObserved) == false)
	{
		// ���� ���߿� ������ Pioneer
		IdCurrentlyBeingObserved = keys.Top();
	}

	if (Pioneers.Contains(IdCurrentlyBeingObserved))
	{
		ViewpointState = EViewpointState::Observation;

		ViewTarget = Pioneers[IdCurrentlyBeingObserved];

		// ���� �����ϴ� ��Ÿ���� ������� �ٷ� �̵��ǹǷ� WorldViewCameraOfNextPioneer�� ���ļ� �̵��ǵ��� �մϴ�.
		if (PioneerController->GetViewTarget() == WorldViewCameraOfCurrentPioneer)
		{
			FTransform transform = WorldViewCameraOfCurrentPioneer->GetActorTransform();
			WorldViewCameraOfNextPioneer->SetActorTransform(transform);
			PioneerController->SetViewTargetWithBlend(WorldViewCameraOfNextPioneer, 0.0f);
		}

		// ī�޶� ��ġ ����
		TickOfObservation();

		PioneerController->SetViewTargetWithBlend(WorldViewCameraOfCurrentPioneer, 1.0f);
	}
}
void APioneerManager::ObserveLeft()
{
	if (!PioneerController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::ObserveLeft(...)> if (!PioneerController)"));
#endif	
		return;
	}
	/***********************************************************************/

	TArray<int32> keys;
	Pioneers.GetKeys(keys);

	int32 currentIdx = keys.Find(IdCurrentlyBeingObserved);

	currentIdx--;

	// �� ó���̸� ������ ��ȯ�մϴ�.
	if (currentIdx < 0)
	{
		currentIdx = keys.Num() - 1;
	}

	if (keys.IsValidIndex(currentIdx))
		IdCurrentlyBeingObserved = keys[currentIdx];
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::ObserveLeft(...)> if (keys.IsValidIndex(currentIdx) == false)"));
#endif	
	}

	Observation();
}
void APioneerManager::ObserveRight()
{
	if (!PioneerController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::ObserveRight(...)> if (!PioneerController)"));
#endif	
		return;
	}
	/***********************************************************************/


	TArray<int32> keys;
	Pioneers.GetKeys(keys);

	int32 currentIdx = keys.Find(IdCurrentlyBeingObserved);

	currentIdx++;

	// �� ���̸� ó������ ��ȯ�մϴ�.
	if (currentIdx >= keys.Num())
	{
		currentIdx = 0;
	}

	if (keys.IsValidIndex(currentIdx))
		IdCurrentlyBeingObserved = keys[currentIdx];
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::ObserveRight(...)> if (keys.IsValidIndex(currentIdx) == false)"));
#endif	
	}
	Observation();
}

void APioneerManager::SwitchToFreeViewpoint()
{
	// �̹� ���������̸� ���̻� �������� �ʽ��ϴ�.
	if (ViewpointState == EViewpointState::Free)
	{
		return;
	}
	if (!PioneerController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::SwitchToFreeViewpoint()> if (!PioneerController)"));
#endif	
		return;
	}
	if (!FreeViewCamera)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::SwitchToFreeViewpoint()> if (!FreeViewCamera)"));
#endif			
		return;
	}

	/***********************************************************************/

	FVector location;

	if (ViewTarget)
	{
		if (ViewTarget->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<APioneerManager::SwitchToFreeViewpoint()> if (ViewTarget->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)"));
#endif	
			return;
		}

		location = ViewTarget->GetActorLocation();
	}
	else
	{
		if (!PioneerController->GetViewTarget())
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<APioneerManager::SwitchToFreeViewpoint()> if (!PioneerController->GetViewTarget())"));
#endif	
			return;
		}

		location = PioneerController->GetViewTarget()->GetActorLocation();
	}

	location.Z = 2500.0f;

	FreeViewCamera->SetActorLocation(location);
	FreeViewCamera->SetActorRotation(FRotator(-45.0f, 0.0f, 0.0f));

	PioneerController->SetViewTargetWithBlend(FreeViewCamera, 1.0f);

	// UI ����
	if (InGameWidget)
	{
		InGameWidget->SetArrowButtonsVisibility(false);
		InGameWidget->SetPossessButtonVisibility(false);
		InGameWidget->SetFreeViewpointButtonVisibility(false);
		InGameWidget->SetObservingButtonVisibility(true);
	}

	ViewpointState = EViewpointState::Free;
}

void APioneerManager::PossessObservingPioneer()
{
	if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneer()> if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)"));
#endif	
		return;
	}
	if (!ViewTarget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneer()> if (!ViewTarget)"));
#endif	
		return;
	}

	if (!Pioneers.Contains(IdCurrentlyBeingObserved))
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneer()> if (!Pioneers.Contains(IdCurrentlyBeingObserved))"));
#endif	
		return;
	}

	if (Pioneers[IdCurrentlyBeingObserved]->bDying)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneer()> if (Pioneers[IdCurrentlyBeingObserved]->bDying)"));
#endif	
		return;
	}
	/***********************************************************************/

	// UI ����
	if (InGameWidget)
	{
		InGameWidget->SetArrowButtonsVisibility(false);
		InGameWidget->SetPossessButtonVisibility(false);
		InGameWidget->SetFreeViewpointButtonVisibility(false);
		InGameWidget->SetObservingButtonVisibility(false);
	}

	ViewpointState = EViewpointState::WaitingPermission;


	cInfoOfPioneer_Socket socket;
	socket.ID = IdCurrentlyBeingObserved;
	socket.NameOfID = ClientSocket->CopyMyInfo().ID;


	if (ServerSocketInGame->IsServerOn())
	{
		socket.SocketID = ServerSocketInGame->SocketID;

		// ���� �� �� �ִ��� Ȯ��
		bool result = ServerSocketInGame->PossessingPioneer(socket);

		if (result)
		{
			// ����
			PossessObservingPioneerByRecv(socket);

			//if (APioneer* pioneer = Pioneers[socket.ID])
			//	pioneer->SetInfoOfPioneer_Socket(socket);
		}
		else
		{
			// �ٽ� �������
			ViewpointState = EViewpointState::Observation;

			// UI ����
			if (InGameWidget)
			{
				InGameWidget->SetArrowButtonsVisibility(true);
				InGameWidget->SetPossessButtonVisibility(true);
				InGameWidget->SetFreeViewpointButtonVisibility(true);
				InGameWidget->SetObservingButtonVisibility(false);
			}
		}

		return;
	}

	if (ClientSocketInGame->IsClientSocketOn())
	{
		ClientSocketInGame->SendPossessPioneer(socket);

		return;
	}
	

	// �̱��÷��̿��� �ٷ� ����
	PossessObservingPioneerByRecv(socket);
}
void APioneerManager::PossessObservingPioneerByRecv(const class cInfoOfPioneer_Socket& Socket)
{
	if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneerByRecv(...)> if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)"));
#endif	
		return;
	}
	if (!PioneerController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneerByRecv(...)> if (!PioneerController)"));
#endif	
		return;
	}
	if (!Pioneers.Contains(Socket.ID))
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneerByRecv(...)> if (!Pioneers.Contains(Socket.ID))"));
#endif	
		
		// �������� ������ �������� ���� ��ȯ�մϴ�.
		SwitchToFreeViewpoint();
		return;
	}
	/***********************************************************************/

	// UI ����
	if (InGameWidget)
	{
		InGameWidget->SetArrowButtonsVisibility(false);
		InGameWidget->SetPossessButtonVisibility(false);
		InGameWidget->SetFreeViewpointButtonVisibility(false);
		InGameWidget->SetObservingButtonVisibility(false);
		
		InGameWidget->SetBuildingBoxVisibility(true);
	}

	if (APioneer* pioneer = Pioneers[Socket.ID])
	{
		IdCurrentlyBeingObserved = 0;

		if (ClientSocketInGame->IsClientSocketOn())
		{
			pioneer->SetInfoOfPioneer_Socket(const_cast<cInfoOfPioneer_Socket&>(Socket));
		}
		
		PioneerOfPlayer = pioneer;

		ViewpointState = EViewpointState::Pioneer;

		TickOfObservation();

		PioneerController->SetViewTargetWithBlend(CameraOfCurrentPioneer, 1.0f);

		if (GetWorldTimerManager().IsTimerActive(TimerOfPossessPioneer))
			GetWorldTimerManager().ClearTimer(TimerOfPossessPioneer);

		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("SetTimerForPossessPioneer"), pioneer); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		GetWorldTimerManager().SetTimer(TimerOfPossessPioneer, timerDel, 1.2f, false);
	}
	else
	{
		// �������� ������ �������� ���� ��ȯ�մϴ�.
		SwitchToFreeViewpoint();
	}
}

void APioneerManager::SetTimerForPossessPioneer(class APioneer* Pioneer)
{
	if (!Pioneer)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::SetTimerForPossessPioneer(...)> if (!Pioneer)"));
#endif	
		return;
	}
	if (!PioneerController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::SetTimerForPossessPioneer(...)> if (!PioneerController)"));
#endif	
		return;
	}

	// ī�޶�Ÿ�� Ȱ��ȭ�� �ڵ��������� �ʵ��� �մϴ�. (true�� ��, ���� �����ϸ� �ڵ����� ��Ÿ���� ����?)
	PioneerController->bAutoManageActiveCameraTarget = true;
	
	// AI Controller�� �����մϴ�.
	Pioneer->UnPossessAIController();

	PioneerController->OnPossess(Pioneer);
}

void APioneerManager::TickOfViewTarget()
{
	if (!InGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfViewTarget()> if (!InGameWidget)"));
#endif	
		return;
	}
	if (!ViewTarget)
	{
		InGameWidget->SetPioneerBoxVisibility(false);
		InGameWidget->SetWeaponBoxVisibility(false);

		return;
	}

	// ����â
	InGameWidget->SetPioneerBoxVisibility(true);
	InGameWidget->SetTextOfPioneerBox(ViewTarget);

	if (AWeapon* weapon = ViewTarget->GetCurrentWeapon())
	{
		InGameWidget->SetWeaponBoxVisibility(true);
		InGameWidget->SetTextOfWeaponBox(weapon);
	}
	else
	{
		InGameWidget->SetWeaponBoxVisibility(false);
	}

	// �̴ϸ�
	if (SceneCapture2D)
	{
		FVector location = SceneCapture2D->GetActorLocation();
		location.X = ViewTarget->GetActorLocation().X;
		location.Y = ViewTarget->GetActorLocation().Y;
		SceneCapture2D->SetActorLocation(location);
	}
}

void APioneerManager::TickOfResources()
{
	if (!InGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfResources()> if (!InGameWidget)"));
#endif	
		return;
	}

	InGameWidget->SetTextOfResources(Pioneers.Num(), Resources);
}

/*** APioneerManager : End ***/


