// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Etc/WorldViewCameraActor.h"
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
APioneerManager::APioneerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	SwitchState = ESwitchState::Switchable;
}

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
	
	// UWorld에서 APioneerController를 찾습니다.
	if (!PioneerCtrl)
	{
		for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
		{
			PioneerCtrl = *ActorItr;
		}
	}

	// UWorld에서 APioneer를 찾고 TArray에 추가합니다.
	for (TActorIterator<APioneer> ActorItr(world); ActorItr; ++ActorItr)
	{
		if (Pioneers.Contains(*ActorItr) == false) // 이미 추가되어있지 않다면
			Pioneers.Add(*ActorItr);

		ActorItr->SetPioneerManager(this);
	}
}

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
/*** Basic Function : End ***/


/*** APioneerManager : Start ***/
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

void APioneerManager::SwitchOtherPioneer(APioneer* CurrentPioneer, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
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
	7. 
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

			// FindTargetViewActor의 WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer 전환 문제를 해결하기 위해,
			// WorldViewCameraOfNextPioneer의 위치를 WorldViewCameraOfCurrentPioneer에 복사하고 먼저 WorldViewCameraOfCurrentPioneer로 전환
			WorldViewCameraOfCurrentPioneer->SetActorTransform(WorldViewCameraOfNextPioneer->GetActorTransform());
			PioneerCtrl->SetViewTarget(WorldViewCameraOfCurrentPioneer);

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
	GetWorldTimerManager().SetTimer(TimerHandleOfSwitchFinish, timerDel, 0.1f, false, BlendTime + 0.25f);
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
			// FindTargetViewActor의 WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer 전환 문제를 해결하기 위해,
			// WorldViewCameraOfNextPioneer의 위치를 WorldViewCameraOfCurrentPioneer에 복사하고 먼저 WorldViewCameraOfCurrentPioneer로 전환
			WorldViewCameraOfCurrentPioneer->SetActorTransform(WorldViewCameraOfNextPioneer->GetActorTransform());
			SwitchViewTarget(WorldViewCameraOfCurrentPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);

			UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchFinish: if (pioneer->bDying || pioneer->IsActorBeingDestroyed())"));

			FTimerDelegate timerDel;
			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
			GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);

			return;
		}

		PossessPioneer(Cast<APioneer>(TargetViewActor));
		SwitchState = ESwitchState::Switchable;
	}

}

void APioneerManager::PossessPioneer(APioneer* Pioneer)
{
	UE_LOG(LogTemp, Warning, TEXT("APioneerManager::PossessPioneer"));

	// 플레이어가 조종하는 개척자를 저장합니다.
	PioneerOfPlayer = Cast<APioneer>(TargetViewActor);

	// TargetViewActor는 다시 초기화합니다.
	TargetViewActor = nullptr;

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
/*** APioneerManager : End ***/
