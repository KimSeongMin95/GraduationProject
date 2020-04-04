// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
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
/*** 직접 정의한 헤더 전방 선언 : End ***/

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

	// Pioneer의 Default가 0이므로 꼭 1부터 시작해야 합니다.
	KeyID = 1;

	IdCurrentlyBeingObserved = 0;

	Resources = cInfoOfResources();

	SceneCapture2D = nullptr;

	BuildingManager = nullptr;
}

void APioneerManager::BeginPlay()
{
	Super::BeginPlay();

	// 카메라들을 생성합니다.
	SpawnWorldViewCameraActor(&FreeViewCamera, FTransform(FRotator(-30.0f, 45.0f, 0.0f), FVector(-9969.8f, -9484.5f, 1441.6f)));

	SpawnWorldViewCameraActor(&CameraOfCurrentPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));
	SpawnWorldViewCameraActor(&WorldViewCameraOfCurrentPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));
	SpawnWorldViewCameraActor(&WorldViewCameraOfNextPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));
	SpawnWorldViewCameraActor(&CameraOfNextPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));


	//////////////////////////
	// 네트워크
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
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	*WorldViewCameraActor = world->SpawnActor<AWorldViewCameraActor>(AWorldViewCameraActor::StaticClass(), myTrans, SpawnParams);

	// 종횡비 제한을 끔으로써 화면이 깨지는 현상을 방지합니다.
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

		// 이미 추가되어있지 않다면
		if (Pioneers.Contains(KeyID) == false)
		{
			Pioneers.Add(KeyID, *ActorItr);
			ActorItr->ID = KeyID;

			if (ServerSocketInGame)
			{
				// 이미 생성된 Pioneer를 게임서버에 알립니다.
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
//// ViewTarget과 Possess 변환
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
//	// 전환할 APioneer를 찾으면
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
//		timerDel.BindUFunction(this, FName("SwitchNext"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
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
//			// FindTargetViewActor의 WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer 전환 문제를 해결하기 위해,
//			// WorldViewCameraOfNextPioneer의 위치를 WorldViewCameraOfCurrentPioneer에 복사하고 먼저 WorldViewCameraOfCurrentPioneer로 전환
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
//			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
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
//	timerDel.BindUFunction(this, FName("SwitchFinish"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
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
//			// FindTargetViewActor의 WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer 전환 문제를 해결하기 위해,
//			// WorldViewCameraOfNextPioneer의 위치를 WorldViewCameraOfCurrentPioneer에 복사하고 먼저 WorldViewCameraOfCurrentPioneer로 전환
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
//			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
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
//	// 플레이어가 조종하는 개척자를 저장합니다.
//	PioneerOfPlayer = Cast<APioneer>(TargetViewActor);
//
//	// TargetViewActor는 다시 초기화합니다.
//	TargetViewActor = nullptr;
//
//	// PioneerController가 존재하는지 확인합니다.
//	if (!PioneerController)
//	{
//		printf_s("[ERROR] <APioneerManager::PossessPioneer(...)> if (!PioneerController)\n");
//		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::PossessPioneer(...)> if (!PioneerController)"));
//		return;
//	}
//
//	//// PioneerController가 Pawn을 소유하고 있으면 소멸시킵니다.
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
//			// 위치 조정
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
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	/* SpawnParams.Instigator = Instigator;
	언리얼 엔진의 게임 프레임웍의 모든 액터에는 가해자(Instigator)라는 변수가 설정되어 있습니다.
	이는 나에게 데미지를 가한 액터의 정보를 보관 용도로 사용되는데,
	반드시 데미지를 가한 액터만 보관하지는 않습니다.
	예를 들어서 자신을 스폰한 액터라던지, 탐지할 적 등 주요 대상을 저장하는데 유용하게 사용할 수 있습니다.
	*/
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.
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

	// 이미 추가되어있지 않다면
	if (Pioneers.Contains(KeyID) == false)
	{
		pioneer->ID = KeyID;
		Pioneers.Add(KeyID, pioneer);
	}

	if (ServerSocketInGame)
	{
		// Pioneer 생성을 게임클라이언트들에게 알립니다.
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
	// 이미 존재하면 생성하지 않고 값만 설정합니다.
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

	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	APioneer* pioneer = world->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);

	// 충돌나면 위치를 조정
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
//	/* SwitchPawn 절차
//	1. 즉시 CurrentPioneer의 카메라에서 CameraOfCurrentPioneer로 전환
//	2. SwitchTime 동안 CameraOfCurrentPioneer에서 WorldViewCameraOfCurrentPioneer로 전환
//	3. 전환시키기에 마땅한 Pioneer 탐색
//	4. SwitchTime 동안 WorldViewCameraOfCurrentPioneer에서 WorldViewCameraOfNextPioneer로 전환
//	5. 탐색한 Pioneer가 살아있다면 SwitchTime 동안 WorldViewCameraOfNextPioneer에서 CameraOfNextPioneer로 전환
//	6. 전환도중 Pioneer가 죽으면 다시 3번으로 회귀
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
//		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
//		GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
//	}
//	else
//	{
//		printf_s("[INFO] <APioneerManager::SwitchOtherPioneer(...)> else\n");
//		UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchOtherPioneer(...)> else"));
//
//		FTimerDelegate timerDel;
//		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
//		GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
//	}
//}


void APioneerManager::TickOfObservation()
{
	// 빙의한 Pioneer가 있으면 실행하지 않습니다.
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

	// Pioneer가 0명이라면 자유시점으로 전환합니다.
	if (keys.Num() == 0)
	{
		SwitchToFreeViewpoint();
		return;
	}

	// UI 설정
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
		// 가장 나중에 생성된 Pioneer
		IdCurrentlyBeingObserved = keys.Top();
	}

	if (Pioneers.Contains(IdCurrentlyBeingObserved))
	{
		ViewpointState = EViewpointState::Observation;

		ViewTarget = Pioneers[IdCurrentlyBeingObserved];

		// 만약 변경하는 뷰타켓이 같을경우 바로 이동되므로 WorldViewCameraOfNextPioneer를 거쳐서 이동되도록 합니다.
		if (PioneerController->GetViewTarget() == WorldViewCameraOfCurrentPioneer)
		{
			FTransform transform = WorldViewCameraOfCurrentPioneer->GetActorTransform();
			WorldViewCameraOfNextPioneer->SetActorTransform(transform);
			PioneerController->SetViewTargetWithBlend(WorldViewCameraOfNextPioneer, 0.0f);
		}

		// 카메라 위치 조정
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

	// 맨 처음이면 끝으로 순환합니다.
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

	// 맨 끝이면 처음으로 순환합니다.
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
	// 이미 자유시점이면 더이상 진행하지 않습니다.
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

	// UI 설정
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

	// UI 설정
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

		// 빙의 할 수 있는지 확인
		bool result = ServerSocketInGame->PossessingPioneer(socket);

		if (result)
		{
			// 빙의
			PossessObservingPioneerByRecv(socket);

			//if (APioneer* pioneer = Pioneers[socket.ID])
			//	pioneer->SetInfoOfPioneer_Socket(socket);
		}
		else
		{
			// 다시 관전모드
			ViewpointState = EViewpointState::Observation;

			// UI 설정
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
	

	// 싱글플레이에선 바로 빙의
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
		
		// 존재하지 않으면 자유시점 모드로 전환합니다.
		SwitchToFreeViewpoint();
		return;
	}
	/***********************************************************************/

	// UI 설정
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
		timerDel.BindUFunction(this, FName("SetTimerForPossessPioneer"), pioneer); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
		GetWorldTimerManager().SetTimer(TimerOfPossessPioneer, timerDel, 1.2f, false);
	}
	else
	{
		// 존재하지 않으면 자유시점 모드로 전환합니다.
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

	// 카메라타겟 활성화를 자동관리하지 않도록 합니다. (true일 때, 폰에 빙의하면 자동으로 뷰타겟을 변경?)
	PioneerController->bAutoManageActiveCameraTarget = true;
	
	// AI Controller를 해제합니다.
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

	// 상태창
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

	// 미니맵
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


