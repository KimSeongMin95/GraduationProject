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

	WorldViewCam = nullptr;
	PioneerCtrl = nullptr;
}

// Called when the game starts or when spawned
void APioneerManager::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	// UWorld에서 AWorldViewCameraActor를 찾습니다.
	if (WorldViewCam == nullptr)
	{
		for (TActorIterator<AWorldViewCameraActor> ActorItr(world); ActorItr; ++ActorItr)
		{
			WorldViewCam = *ActorItr;
		}
	}

	// UWorld에서 APioneerController를 찾습니다.
	if (PioneerCtrl == nullptr)
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

}

/** APioneer 객체를 생성합니다. */
void APioneerManager::SpawnPioneer(FTransform Transform)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
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
	
	Pioneers.Add(World->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams));
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
void APioneerManager::SwitchPawn(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	if (PioneerCtrl->GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchPawn PioneerCtrl->GetPawn()"));
		return;
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
	
	// 먼저 WorldViewCam로 카메라를 변경합니다.
	if (PioneerCtrl && WorldViewCam)
		PioneerCtrl->SetViewTargetWithBlend(WorldViewCam, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	else
		UE_LOG(LogTemp, Warning, TEXT("!(PioneerCtrl && WorldViewCam)"));

	// AI인 Pioneer를 찾습니다.
	APioneer* Pawn = nullptr;
	for (auto& pioneer : Pioneers)
	{
		if (pioneer->SocketID == -1)
		{
			Pawn = pioneer;
			break;
		}
	}

	FTimerHandle timer1;
	FTimerDelegate timerDel1;
	// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
	timerDel1.BindUFunction(this, FName("SwitchViewTarget"), Pawn, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	GetWorldTimerManager().SetTimer(timer1, timerDel1, BlendTime, false);

	FTimerHandle timer2;
	FTimerDelegate timerDel2;
	// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
	timerDel2.BindUFunction(this, FName("PossessPioneer"), Pawn);
	GetWorldTimerManager().SetTimer(timer2, timerDel2, BlendTime * 2.0f, false);
}

/** 다른 폰의 카메라로 변경하는 함수입니다. */
void APioneerManager::SwitchViewTarget(APioneer* Pioneer, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	if (!Pioneer)
	{
		UE_LOG(LogTemp, Warning, TEXT("!Pioneer"));
		return;
	}

	PioneerCtrl->SetViewTargetWithBlend(Pioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	
}

/** 다른 폰을 Possess() 합니다. */
void APioneerManager::PossessPioneer(APioneer* Pioneer)
{
	if (!Pioneer)
	{
		for (auto& pioneer : Pioneers)
		{
			if (pioneer->SocketID == -1)
			{
				Pioneer = pioneer;
				break;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::PossessPioneer => !Pioneer"));
	}

	// PioneerCtrl가 존재하는지 확인합니다.
	if (!PioneerCtrl)
	{
		UE_LOG(LogTemp, Warning, TEXT("!PioneerCtrl"));
			return;
	}

	// PioneerCtrl가 Pawn을 소유하고 있으면 먼저 해제합니다.
	if (PioneerCtrl->GetPawn())
	{
		PioneerCtrl->UnPossess();
	}

	// 이제부터 PioneerCtrl가 TmapPioneers[ID]를 조종합니다.
	PioneerCtrl->Possess(Pioneer);
	playerPioneer = Pioneer;
}