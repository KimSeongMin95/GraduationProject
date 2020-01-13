// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Etc/WorldViewCameraActor.h"
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
APioneerManager::APioneerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComp;

	SwitchState = ESwitchState::Switchable;
}

// Called when the game starts or when spawned
void APioneerManager::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::BeginPlay: !world"));
		return;
	}

	for (TActorIterator<AWorldViewCameraActor> ActorItr(world); ActorItr; ++ActorItr)
	{
		if ((*ActorItr)->GetName() == "WorldViewCamera")
		{
			WorldViewCamera = *ActorItr;
		}

		else if ((*ActorItr)->GetName() == "CameraOfCurrentPioneer")
		{
			CameraOfCurrentPioneer = *ActorItr;
		}
		else if ((*ActorItr)->GetName() == "WorldViewCameraOfCurrentPioneer")
		{
			WorldViewCameraOfCurrentPioneer = *ActorItr;
		}

		else if ((*ActorItr)->GetName() == "WorldViewCameraOfNextPioneer")
		{
			WorldViewCameraOfNextPioneer = *ActorItr;
		}
		else if ((*ActorItr)->GetName() == "CameraOfNextPioneer")
		{
			CameraOfNextPioneer = *ActorItr;
		}
	}

	TargetViewActor = WorldViewCamera;
	
	// UWorld에서 APioneerController를 찾습니다.
	if (!PioneerCtrl)
	{
		for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
		{
			PioneerCtrl = *ActorItr;
		}
	}
}

// Called every frame
void APioneerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SwitchState == ESwitchState::Next || SwitchState == ESwitchState::Finish)
	{
		if (TargetViewActor)
		{
			// 위치 조정
			if (TargetViewActor->IsA(APioneer::StaticClass()))
			{
				Cast<APioneer>(TargetViewActor)->CopyTopDownCameraTo(CameraOfNextPioneer);
			}
		}
	}
}

/** APioneer 객체를 생성합니다. */
void APioneerManager::SpawnPioneer(FTransform Transform)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SpawnPioneer: !World"));
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
	
	World->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);
}

APioneer* APioneerManager::GetPioneerBySocketID(int SocketID)
{
	for (auto& pioneer : Pioneers)
	{
		if (pioneer->SocketID == SocketID)
			return pioneer;
	}

	return nullptr;
}

/** 다른 폰으로 변경하는 함수입니다. */
void APioneerManager::SwitchPawn(APioneer* CurrentPioneer, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
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
	
	/* SwitchPawn 절차
	1. 즉시 CurrentPioneer의 카메라에서 CameraOfCurrentPioneer로 전환
	2. SwitchTime 동안 CameraOfCurrentPioneer에서 WorldViewCameraOfCurrentPioneer로 전환
	3. 전환시키기에 마땅한 Pioneer 탐색
	4. SwitchTime 동안 WorldViewCameraOfCurrentPioneer에서 WorldViewCameraOfNextPioneer로 전환
	5. 탐색한 Pioneer가 살아있다면 SwitchTime 동안 WorldViewCameraOfNextPioneer에서 CameraOfNextPioneer로 전환
	6. 전환도중 Pioneer가 죽으면 다시 3번으로 회귀
	*/

	if (SwitchState != ESwitchState::Switchable)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchPawn: if (SwitchState != ESwitchState::Switchable)"));
		return;
	}

	if (!PioneerCtrl)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchPawn: if (!PioneerCtrl)"));
		return;
	}

	if (CurrentPioneer)
	{
		if (CurrentPioneer->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)
			return;

		PioneerCtrl->SetViewTargetWithBlend(CameraOfCurrentPioneer);

		FVector location = CurrentPioneer->GetActorLocation();
		location.Z = 5000.0f;
		WorldViewCameraOfCurrentPioneer->SetActorLocation(location);

		SwitchViewTarget(WorldViewCameraOfCurrentPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
		SwitchState = ESwitchState::Current;

		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
		GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchPawn: else"));

		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
		GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
	}
}

void APioneerManager::SwitchViewTarget(AActor* Actor, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchViewTarget: !Actor"));
		return;
	}
	
	if (!PioneerCtrl)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchViewTarget: !PioneerCtrl"));
		return;
	}

	PioneerCtrl->SetViewTargetWithBlend(Actor, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchViewTarget: %s, BlendTime: %f"), *Actor->GetName(), BlendTime);
}

void APioneerManager::FindTargetViewActor(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	UE_LOG(LogTemp, Warning, TEXT("APioneerManager::FindTargetViewActor"));
	SwitchState = ESwitchState::FindTargetViewActor;

	TargetViewActor = nullptr;

	for (auto& pioneer : Pioneers)
	{
		if (!pioneer)
			continue;

		if (pioneer->bDying || pioneer->IsActorBeingDestroyed())
			continue;

		if (pioneer->SocketID == -1)
			TargetViewActor = pioneer;
	}


	// 전환할 APioneer를 찾으면
	if (TargetViewActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::FindTargetViewActor: if (TargetViewActor)"));

		FVector location = TargetViewActor->GetActorLocation();
		location.Z = 5000.0f;
		WorldViewCameraOfNextPioneer->SetActorLocation(location);

		SwitchViewTarget(WorldViewCameraOfNextPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);


		GetWorldTimerManager().ClearTimer(TimerHandleOfFindTargetViewActor);
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchViewTarget: GetWorldTimerManager().ClearTimer(TimerHandleOfFindTargetViewActor);"));


		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("SwitchNext"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
		GetWorldTimerManager().SetTimer(TimerOfSwitchNext, timerDel, 0.1f, false, BlendTime + 0.5f);
	}
}

void APioneerManager::SwitchNext(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	UE_LOG(LogTemp, Warning, TEXT("APioneerManager::Next"));
	SwitchState = ESwitchState::Next;

	if (!TargetViewActor)
	{
		SwitchState = ESwitchState::Switchable;
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::Next: !TargetViewActor"));
		return;
	}

	if (APioneer* pioneer = Cast<APioneer>(TargetViewActor))
	{
		if (pioneer->bDying || pioneer->IsActorBeingDestroyed())
		{
			UE_LOG(LogTemp, Warning, TEXT("APioneerManager::Next: pioneer->bDying || pioneer->IsActorBeingDestroyed()"));

			FTimerDelegate timerDel;
			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
			GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true);

			return;
		}

		pioneer->CopyTopDownCameraTo(CameraOfCurrentPioneer);
	}

	if (!PioneerCtrl)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchViewTarget: !PioneerCtrl"));
		return;
	}

	PioneerCtrl->SetViewTargetWithBlend(CameraOfNextPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchViewTarget: %s, BlendTime: %f"), *CameraOfNextPioneer->GetName(), BlendTime);

	FTimerDelegate timerDel;
	timerDel.BindUFunction(this, FName("SwitchFinish"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
	GetWorldTimerManager().SetTimer(TimerHandleOfSwitchFinish, timerDel, 0.1f, false, BlendTime + 0.5f);
}

void APioneerManager::SwitchFinish(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchFinish"));
	SwitchState = ESwitchState::Finish;

	if (!TargetViewActor)
	{
		SwitchState = ESwitchState::Switchable;
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchFinish: if (!TargetViewActor)"));
		return;
	}
	
	if (APioneer* pioneer = Cast<APioneer>(TargetViewActor))
	{
		if (pioneer->bDying || pioneer->IsActorBeingDestroyed())
		{
			// 일단 다시 올라가서 보기
			SwitchViewTarget(WorldViewCameraOfNextPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);

			UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchFinish: if (pioneer->bDying || pioneer->IsActorBeingDestroyed())"));

			FTimerDelegate timerDel;
			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
			GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true);

			return;
		}

		PossessPioneer(Cast<APioneer>(TargetViewActor));
		SwitchState = ESwitchState::Switchable;
	}

}


/** 다른 폰을 Possess() 합니다. */
void APioneerManager::PossessPioneer(APioneer* Pioneer)
{
	UE_LOG(LogTemp, Warning, TEXT("APioneerManager::PossessPioneer"));

	// PioneerCtrl가 존재하는지 확인합니다.
	if (!PioneerCtrl)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::PossessPioneer: !PioneerCtrl"));
			return;
	}

	// PioneerCtrl가 Pawn을 소유하고 있으면 소멸시킵니다.
	if (PioneerCtrl->GetPawn())
		PioneerCtrl->GetPawn()->Destroy();

	PioneerCtrl->Possess(Pioneer);
	UE_LOG(LogTemp, Warning, TEXT("APioneerManager::PossessPioneer: PioneerCtrl->Possess(Pioneer);"));
}


//// Fill out your copyright notice in the Description page of Project Settings.
//
//#include "PioneerManager.h"
//
///*** 직접 정의한 헤더 전방 선언 : Start ***/
//#include "Etc/WorldViewCameraActor.h"
//#include "Character/Pioneer.h"
//#include "Controller/PioneerController.h"
//#include "Controller/PioneerAIController.h"
///*** 직접 정의한 헤더 전방 선언 : End ***/
//
//// Sets default values
//APioneerManager::APioneerManager()
//{
//	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
//	RootComponent = SceneComp;
//
//	//InitUI();
//}
//
//// Called when the game starts or when spawned
//void APioneerManager::BeginPlay()
//{
//	Super::BeginPlay();
//
//	UWorld* const world = GetWorld();
//	if (!world)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
//		return;
//	}
//
//
//	for (TActorIterator<AWorldViewCameraActor> ActorItr(world); ActorItr; ++ActorItr)
//	{
//		if ((*ActorItr)->GetName() == "WorldViewCamera")
//		{
//			WorldViewCamera = *ActorItr;
//		}
//		if ((*ActorItr)->GetName() == "WorldViewCamFirst")
//		{
//			WorldViewCamFirst = *ActorItr;
//		}
//		if ((*ActorItr)->GetName() == "WorldViewCamSecond")
//		{
//			WorldViewCamSecond = *ActorItr;
//		}
//	}
//
//	TargetViewActor = WorldViewCamera;
//
//
//	// UWorld에서 APioneerController를 찾습니다.
//	if (PioneerCtrl == nullptr)
//	{
//		for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
//		{
//			PioneerCtrl = *ActorItr;
//		}
//	}
//
//	//BeginPlayUI();
//}
//
//// Called every frame
//void APioneerManager::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//
//	/*if (PioneerCtrl)
//	{
//		if (PioneerCtrl->GetPawn())
//		{
//			if (PioneerCtrl->GetPawn()->IsActorBeingDestroyed() == false)
//			{
//				FVector location = PioneerCtrl->GetPawn()->GetActorLocation();
//				location.Z = 5000.0f;
//				WorldViewCamFirst->SetActorLocation(location);
//			}
//		}
//	}*/
//
//	if (TargetViewActor == nullptr || TargetViewActor->IsActorBeingDestroyed())
//	{
//		for (auto& pioneer : Pioneers)
//		{
//			// pioneer가 nullptr면 건너띄기
//			if (!pioneer)
//				continue;
//
//			if (pioneer->SocketID == -1 && pioneer->bDying == false)
//			{
//				TargetViewActor = pioneer;
//				break;
//			}
//		}
//
//		if (TargetViewActor)
//		{
//			SwitchPawn(0.5f);
//		}
//		else
//		{
//			TargetViewActor = WorldViewCamera;
//			SwitchViewTarget(TargetViewActor, 0.5f);
//		}
//	}
//
//}
//
///** APioneer 객체를 생성합니다. */
//void APioneerManager::SpawnPioneer(FTransform Transform)
//{
//	UWorld* const World = GetWorld();
//	if (!World)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
//		return;
//	}
//
//	FTransform myTrans = Transform;
//
//	FActorSpawnParameters SpawnParams;
//	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
//	SpawnParams.Owner = this;
//	SpawnParams.Instigator = Instigator;
//	/* SpawnParams.Instigator = Instigator;
//	언리얼 엔진의 게임 프레임웍의 모든 액터에는 가해자(Instigator)라는 변수가 설정되어 있습니다.
//	이는 나에게 데미지를 가한 액터의 정보를 보관 용도로 사용되는데,
//	반드시 데미지를 가한 액터만 보관하지는 않습니다.
//	예를 들어서 자신을 스폰한 액터라던지, 탐지할 적 등 주요 대상을 저장하는데 유용하게 사용할 수 있습니다.
//	*/
//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.
//	///** Fall back to default settings. */
//	//Undefined								UMETA(DisplayName = "Default"),
//	///** Actor will spawn in desired location, regardless of collisions. */
//	//AlwaysSpawn								UMETA(DisplayName = "Always Spawn, Ignore Collisions"),
//	///** Actor will try to find a nearby non-colliding location (based on shape components), but will always spawn even if one cannot be found. */
//	//AdjustIfPossibleButAlwaysSpawn			UMETA(DisplayName = "Try To Adjust Location, But Always Spawn"),
//	///** Actor will try to find a nearby non-colliding location (based on shape components), but will NOT spawn unless one is found. */
//	//AdjustIfPossibleButDontSpawnIfColliding	UMETA(DisplayName = "Try To Adjust Location, Don't Spawn If Still Colliding"),
//	///** Actor will fail to spawn. */
//	//DontSpawnIfColliding					UMETA(DisplayName = "Do Not Spawn"),
//
//	//Pioneers.Add(World->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams));
//	World->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);
//}
//
//APioneer* APioneerManager::GetPioneerBySocketID(int SocketID)
//{
//	for (auto& pioneer : Pioneers)
//	{
//		if (pioneer->SocketID == SocketID)
//			return pioneer;
//	}
//
//	return nullptr;
//}
//
///** 다른 폰으로 변경하는 함수입니다. */
//void APioneerManager::SwitchPawn(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
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
//	//// 먼저 WorldViewCam로 카메라를 변경합니다.
//	//if (PioneerCtrl && WorldViewCam)
//	//	PioneerCtrl->SetViewTargetWithBlend(WorldViewCam, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//	//else
//	//	UE_LOG(LogTemp, Warning, TEXT("!(PioneerCtrl && WorldViewCam)"));
//
//	// 먼저 First 카메라를 옮겨주기
//	if (PioneerCtrl)
//	{
//		if (PioneerCtrl->GetPawn())
//		{
//			if (PioneerCtrl->GetPawn()->IsActorBeingDestroyed() == false)
//			{
//				FVector location = PioneerCtrl->GetPawn()->GetActorLocation();
//				location.Z = 5000.0f;
//				WorldViewCamFirst->SetActorLocation(location);
//
//				SwitchViewTarget(WorldViewCamFirst, BlendTime);
//			}
//		}
//	}
//
//	// AI인 Pioneer를 찾습니다.
//	APioneer* Pawn = nullptr;
//	for (auto& pioneer : Pioneers)
//	{
//		// pioneer가 nullptr면 건너띄기
//		if (!pioneer)
//			continue;
//
//		if (pioneer->SocketID == -1 && pioneer->bDying == false)
//		{
//			Pawn = pioneer;
//			break;
//		}
//	}
//
//	float OneFrameGap = 0.033f;
//
//	// 그다음 Second 카메라를 옮겨주기
//	if (Pawn)
//	{
//		FVector location = Pawn->GetActorLocation();
//		location.Z = 5000.0f;
//		WorldViewCamSecond->SetActorLocation(location);
//
//		FTimerDelegate timerDel;
//		// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
//		timerDel.BindUFunction(this, FName("SwitchViewTarget"), WorldViewCamSecond, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//		GetWorldTimerManager().SetTimer(timer, timerDel, BlendTime + OneFrameGap, false);
//
//		FTimerDelegate timerDel1;
//		// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
//		timerDel1.BindUFunction(this, FName("SwitchViewTarget"), Pawn, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//		GetWorldTimerManager().SetTimer(timer1, timerDel1, BlendTime * 2.0f + OneFrameGap, false);
//
//		FTimerDelegate timerDel2;
//		// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
//		timerDel2.BindUFunction(this, FName("PossessPioneer"), Pawn, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//		GetWorldTimerManager().SetTimer(timer2, timerDel2, BlendTime * 3.0f + OneFrameGap, false);
//	}
//	else
//	{
//		FTimerDelegate timerDel3;
//		// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
//		timerDel3.BindUFunction(this, FName("SwitchViewTarget"), WorldViewCamera, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//		GetWorldTimerManager().SetTimer(timer3, timerDel3, BlendTime + OneFrameGap, false);
//	}
//}
//
///** 다른 폰의 카메라로 변경하는 함수입니다. */
//void APioneerManager::SwitchViewTarget(AActor* Actor, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
//{
//
//
//	if (!Actor)
//	{
//		SwitchViewTarget(WorldViewCamSecond, 1.0f);
//		/*TargetViewActor = nullptr;
//		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchViewTarget: !Actor"));
//		PioneerCtrl->SetViewTargetWithBlend(WorldViewCamera, BlendTime, blendFunc, BlendExp, bLockOutgoing);*/
//		if (GetWorldTimerManager().IsTimerActive(timer))
//			GetWorldTimerManager().ClearTimer(timer);
//		if (GetWorldTimerManager().IsTimerActive(timer1))
//			GetWorldTimerManager().ClearTimer(timer1);
//		if (GetWorldTimerManager().IsTimerActive(timer2))
//			GetWorldTimerManager().ClearTimer(timer2);
//		if (GetWorldTimerManager().IsTimerActive(timer3))
//			GetWorldTimerManager().ClearTimer(timer3);
//		TargetViewActor = nullptr;
//		return;
//	}
//
//	if (Actor->IsA(APioneer::StaticClass()))
//	{
//		if (Cast<APioneer>(Actor)->bDying || Actor->IsActorBeingDestroyed())
//		{
//			/*TargetViewActor = nullptr;
//			UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchViewTarget: !Actor"));
//			PioneerCtrl->SetViewTargetWithBlend(WorldViewCamera, BlendTime, blendFunc, BlendExp, bLockOutgoing);*/
//
//			SwitchViewTarget(WorldViewCamSecond, 1.0f);
//			if (GetWorldTimerManager().IsTimerActive(timer))
//				GetWorldTimerManager().ClearTimer(timer);
//			if (GetWorldTimerManager().IsTimerActive(timer1))
//				GetWorldTimerManager().ClearTimer(timer1);
//			if (GetWorldTimerManager().IsTimerActive(timer2))
//				GetWorldTimerManager().ClearTimer(timer2);
//			if (GetWorldTimerManager().IsTimerActive(timer3))
//				GetWorldTimerManager().ClearTimer(timer3);
//			TargetViewActor = nullptr;
//			return;
//		}
//	}
//
//	TargetViewActor = Actor;
//
//	UE_LOG(LogTemp, Warning, TEXT("SwitchViewTarget: %s, BlendTime: %f"), *Actor->GetName(), BlendTime);
//	PioneerCtrl->SetViewTargetWithBlend(Actor, BlendTime, blendFunc, BlendExp, bLockOutgoing);
//
//}
//
///** 다른 폰을 Possess() 합니다. */
//void APioneerManager::PossessPioneer(APioneer* Pioneer)
//{
//	/*if (!Pioneer)
//	{
//		for (auto& pioneer : Pioneers)
//		{
//			if (pioneer->SocketID == -1 && pioneer->bDead == false)
//			{
//				Pioneer = pioneer;
//				break;
//			}
//		}
//
//		if (!Pioneer)
//		{
//			TargetViewActor = nullptr;
//
//			SwitchViewTarget(WorldViewCamera, 1.0f);
//			return;
//		}
//
//		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::PossessPioneer => !Pioneer"));
//	}
//	else
//	{
//		if (Pioneer->IsActorBeingDestroyed())
//		{
//			for (auto& pioneer : Pioneers)
//			{
//				if (pioneer->SocketID == -1 && pioneer->bDead == false)
//				{
//					Pioneer = pioneer;
//					break;
//				}
//			}
//
//			if (!Pioneer)
//			{
//				TargetViewActor = nullptr;;
//
//				SwitchViewTarget(WorldViewCamera, 1.0f);
//				return;
//			}
//
//		}
//	}
//
//	if (TargetViewActor == nullptr)
//	{
//		for (auto& pioneer : Pioneers)
//		{
//			if (pioneer->SocketID == -1 && pioneer->bDead == false)
//			{
//				TargetViewActor = pioneer;
//				break;
//			}
//		}
//
//		if (TargetViewActor)
//		{
//			SwitchViewTarget(TargetViewActor, 0.0f);
//			PossessPioneer(Cast<APioneer>(TargetViewActor));
//		}
//		else
//		{
//			TargetViewActor = WorldViewCamera;
//			SwitchViewTarget(TargetViewActor, 1.0f);
//		}
//	}*/
//
//	// PioneerCtrl가 존재하는지 확인합니다.
//	if (!PioneerCtrl)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("!PioneerCtrl"));
//		return;
//	}
//
//	// PioneerCtrl가 Pawn을 소유하고 있으면 먼저 해제합니다.
//	if (PioneerCtrl->GetPawn())
//	{
//		APioneer* ToDestroy = Cast<APioneer>(PioneerCtrl->GetPawn());
//
//		PioneerCtrl->UnPossess();
//	}
//
//
//
//	if (!Pioneer)
//	{
//		SwitchViewTarget(WorldViewCamSecond, 1.0f);
//		if (GetWorldTimerManager().IsTimerActive(timer))
//			GetWorldTimerManager().ClearTimer(timer);
//		if (GetWorldTimerManager().IsTimerActive(timer1))
//			GetWorldTimerManager().ClearTimer(timer1);
//		if (GetWorldTimerManager().IsTimerActive(timer2))
//			GetWorldTimerManager().ClearTimer(timer2);
//		if (GetWorldTimerManager().IsTimerActive(timer3))
//			GetWorldTimerManager().ClearTimer(timer3);
//		TargetViewActor = nullptr;
//		return;
//	}
//
//	if (Pioneer->bDying || Pioneer->IsActorBeingDestroyed())
//	{
//		SwitchViewTarget(WorldViewCamSecond, 1.0f);
//		if (GetWorldTimerManager().IsTimerActive(timer))
//			GetWorldTimerManager().ClearTimer(timer);
//		if (GetWorldTimerManager().IsTimerActive(timer1))
//			GetWorldTimerManager().ClearTimer(timer1);
//		if (GetWorldTimerManager().IsTimerActive(timer2))
//			GetWorldTimerManager().ClearTimer(timer2);
//		if (GetWorldTimerManager().IsTimerActive(timer3))
//			GetWorldTimerManager().ClearTimer(timer3);
//		TargetViewActor = nullptr;
//		return;
//	}
//
//	// 이제부터 PioneerCtrl가 TmapPioneers[ID]를 조종합니다.
//	PioneerCtrl->Possess(Pioneer);
//	//playerPioneer = Pioneer;
//}
//
//
//void APioneerManager::InitUI()
//{
//	UIWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIWidgetComponent"));
//	//HelthPointBar = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass());
//	//UIWidgetComponent->SetupAttachment(RootComponent);
//	UIWidgetComponent->bAbsoluteRotation = true; // 절대적인 회전값을 적용합니다.
//
//	UIWidgetComponent->SetOnlyOwnerSee(false);
//	//HelthPointBar->SetIsReplicated(false);
//
//	UIWidgetComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
//	//HelthPointBar->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
//	UIWidgetComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f)); // 항상 플레이어에게 보이도록 회전 값을 World로 해야 함.
//	UIWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
//	UIWidgetComponent->SetDrawSize(FVector2D(1920, 1080));
//
//	// Screen은 뷰포트에서 UI처럼 띄워주는 것이고 World는 게임 내에서 UI처럼 띄워주는 것
//	UIWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
//}
//
//void APioneerManager::BeginPlayUI()
//{
//	UWorld* const world = GetWorld();
//	if (!world)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::BeginPlayHelthPointBar() Failed: UWorld* const World = GetWorld();"));
//		return;
//	}
//
//	/*** 주의: Blueprint 애셋은 뒤에 _C를 붙여줘서 클래스를 가져와줘야 함. ***/
//	FString UIBP_Reference = "WidgetBlueprint'/Game/UI/Minimap_Widget.Minimap_Widget_C'";
//	UClass* UIBP = LoadObject<UClass>(this, *UIBP_Reference);
//
//	// 가져온 WidgetBlueprint를 UWidgetComponent에 바로 적용하지말고 따로 UUserWidget에 저장하여 설정을 한 뒤
//	// UWidgetComponent->SetWidget(저장한 UUserWidget);으로 UWidgetComponent에 적용해야 함.
//	//HelthPointBar->SetWidgetClass(HelthPointBarBP);
//	UIUserWidget = CreateWidget(world, UIBP); // wolrd가 꼭 필요.
//
//	if (UIUserWidget)
//	{
//		UWidgetTree* WidgetTree = UIUserWidget->WidgetTree;
//		if (WidgetTree)
//		{
//			//// 이 방법은 안됨.
//			// ProgreeBar = Cast<UProgressBar>(HelthPointBarUserWidget->GetWidgetFromName(FName(TEXT("ProgressBar_153"))));
//
//			/*ProgressBar = WidgetTree->FindWidget<UProgressBar>(FName(TEXT("ProgressBar_153")));
//			if (ProgressBar == nullptr)
//				UE_LOG(LogTemp, Warning, TEXT("ProgressBar == nullptr"));*/
//		}
//		else
//			UE_LOG(LogTemp, Warning, TEXT("WidgetTree == nullptr"));
//	}
//	else
//		UE_LOG(LogTemp, Warning, TEXT("HelthPointBarUserWidget == nullptr"));
//
//	UIWidgetComponent->SetWidget(UIUserWidget);
//}