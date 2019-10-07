// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Pioneer.h"
#include "PioneerController.h"
#include "WorldViewCameraActor.h"
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
	SwitchTime = 1.5f;
}

// Called when the game starts or when spawned
void APioneerManager::BeginPlay()
{
	Super::BeginPlay();

	SpawnPioneer(1);
	SpawnPioneer(2);

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

	/*** SwitchPawn() temp code : Start ***/
	static float tmp = 0;
	tmp += DeltaTime;
	static int tmpID = 1;

	if (tmp > 15.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("SwitchPawn()"));
		tmp = 0.0f;
		SwitchPawn(tmpID, SwitchTime);

		tmpID++;
		if (tmpID >= 3)
			tmpID = 1;
	}
	/*** SwitchPawn() temp code : End ***/
}

/** APioneer 객체를 생성합니다. */
void APioneerManager::SpawnPioneer(int ID)
{
	if (TmapPioneers.Contains(ID))
	{
		UE_LOG(LogTemp, Warning, TEXT("TmapPioneers.Contains(ID)"));
		return;
	}

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = GetTransform(); // 현재 PioneerManager 객체 위치를 기반으로 합니다.

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

	TmapPioneers.Emplace(ID, World->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams)); // 액터를 객체화 합니다.
}

APioneer* APioneerManager::GetPioneerByID(int ID)
{
	// ID에 해당하는 key가 존재하는지 확인합니다.
	if (!TmapPioneers.Contains(ID))
	{
		UE_LOG(LogTemp, Warning, TEXT("!TmapPioneers.Contains(ID)"));
		return nullptr;
	}

	return TmapPioneers[ID];
}

/** 다른 폰으로 변경하는 함수입니다. */
void APioneerManager::SwitchPawn(int ID, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
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
	
	// 먼저 WorldViewCam로 카메라를 변경합니다.
	if (PioneerCtrl && WorldViewCam)
		PioneerCtrl->SetViewTargetWithBlend(WorldViewCam, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	else
		UE_LOG(LogTemp, Warning, TEXT("!(PioneerCtrl && WorldViewCam)"));

	FTimerHandle timer1;
	FTimerDelegate timerDel1;
	// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
	timerDel1.BindUFunction(this, FName("SwitchViewTarget"), ID, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	GetWorldTimerManager().SetTimer(timer1, timerDel1, BlendTime, false);

	FTimerHandle timer2;
	FTimerDelegate timerDel2;
	// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
	timerDel2.BindUFunction(this, FName("PossessPioneer"), ID);
	GetWorldTimerManager().SetTimer(timer2, timerDel2, BlendTime * 2.0f, false);
}

/** 다른 폰의 카메라로 변경하는 함수입니다. */
void APioneerManager::SwitchViewTarget(int ID, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	// ID에 해당하는 key가 존재하는지 확인합니다.
	if (!TmapPioneers.Contains(ID))
	{
		UE_LOG(LogTemp, Warning, TEXT("!TmapPioneers.Contains(ID)"));
			return;
	}

	// TmapPioneers[ID]의 카메라로 변경합니다.
	PioneerCtrl->SetViewTargetWithBlend(TmapPioneers[ID], BlendTime, blendFunc, BlendExp, bLockOutgoing);
}

/** 다른 폰을 Possess() 합니다. */
void APioneerManager::PossessPioneer(int ID)
{
	// ID에 해당하는 key가 존재하는지 확인합니다.
	if (!TmapPioneers.Contains(ID))
	{
		UE_LOG(LogTemp, Warning, TEXT("!TmapPioneers.Contains(ID)"));
			return;
	}
	// PioneerCtrl가 존재하는지 확인합니다.
	if (!PioneerCtrl)
	{
		UE_LOG(LogTemp, Warning, TEXT("!PioneerCtrl"));
			return;
	}

	// PioneerCtrl가 Pawn을 소유하고 있으면 먼저 해제합니다.
	if (PioneerCtrl->GetPawn())
		PioneerCtrl->UnPossess();

	// 이제부터 PioneerCtrl가 TmapPioneers[ID]를 조종합니다.
	PioneerCtrl->Possess(TmapPioneers[ID]);
}