// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Pioneer.h"
#include "PioneerController.h"
#include "WorldViewCameraActor.h"
/*** ���� ������ ��� ���� ���� : End ***/

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

	// UWorld���� AWorldViewCameraActor�� ã���ϴ�.
	if (WorldViewCam == nullptr)
	{
		for (TActorIterator<AWorldViewCameraActor> ActorItr(world); ActorItr; ++ActorItr)
		{
			WorldViewCam = *ActorItr;
		}
	}

	// UWorld���� APioneerController�� ã���ϴ�.
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

/** APioneer ��ü�� �����մϴ�. */
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

	FTransform myTrans = GetTransform(); // ���� PioneerManager ��ü ��ġ�� ������� �մϴ�.

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

	TmapPioneers.Emplace(ID, World->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams)); // ���͸� ��üȭ �մϴ�.
}

APioneer* APioneerManager::GetPioneerByID(int ID)
{
	// ID�� �ش��ϴ� key�� �����ϴ��� Ȯ���մϴ�.
	if (!TmapPioneers.Contains(ID))
	{
		UE_LOG(LogTemp, Warning, TEXT("!TmapPioneers.Contains(ID)"));
		return nullptr;
	}

	return TmapPioneers[ID];
}

/** �ٸ� ������ �����ϴ� �Լ��Դϴ�. */
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
	
	// ���� WorldViewCam�� ī�޶� �����մϴ�.
	if (PioneerCtrl && WorldViewCam)
		PioneerCtrl->SetViewTargetWithBlend(WorldViewCam, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	else
		UE_LOG(LogTemp, Warning, TEXT("!(PioneerCtrl && WorldViewCam)"));

	FTimerHandle timer1;
	FTimerDelegate timerDel1;
	// �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
	timerDel1.BindUFunction(this, FName("SwitchViewTarget"), ID, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	GetWorldTimerManager().SetTimer(timer1, timerDel1, BlendTime, false);

	FTimerHandle timer2;
	FTimerDelegate timerDel2;
	// �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
	timerDel2.BindUFunction(this, FName("PossessPioneer"), ID);
	GetWorldTimerManager().SetTimer(timer2, timerDel2, BlendTime * 2.0f, false);
}

/** �ٸ� ���� ī�޶�� �����ϴ� �Լ��Դϴ�. */
void APioneerManager::SwitchViewTarget(int ID, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	// ID�� �ش��ϴ� key�� �����ϴ��� Ȯ���մϴ�.
	if (!TmapPioneers.Contains(ID))
	{
		UE_LOG(LogTemp, Warning, TEXT("!TmapPioneers.Contains(ID)"));
			return;
	}

	// TmapPioneers[ID]�� ī�޶�� �����մϴ�.
	PioneerCtrl->SetViewTargetWithBlend(TmapPioneers[ID], BlendTime, blendFunc, BlendExp, bLockOutgoing);
}

/** �ٸ� ���� Possess() �մϴ�. */
void APioneerManager::PossessPioneer(int ID)
{
	// ID�� �ش��ϴ� key�� �����ϴ��� Ȯ���մϴ�.
	if (!TmapPioneers.Contains(ID))
	{
		UE_LOG(LogTemp, Warning, TEXT("!TmapPioneers.Contains(ID)"));
			return;
	}
	// PioneerCtrl�� �����ϴ��� Ȯ���մϴ�.
	if (!PioneerCtrl)
	{
		UE_LOG(LogTemp, Warning, TEXT("!PioneerCtrl"));
			return;
	}

	// PioneerCtrl�� Pawn�� �����ϰ� ������ ���� �����մϴ�.
	if (PioneerCtrl->GetPawn())
		PioneerCtrl->UnPossess();

	// �������� PioneerCtrl�� TmapPioneers[ID]�� �����մϴ�.
	PioneerCtrl->Possess(TmapPioneers[ID]);
}