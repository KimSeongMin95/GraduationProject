// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Etc/WorldViewCameraActor.h"
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
APioneerManager::APioneerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComp;

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


	for (TActorIterator<AWorldViewCameraActor> ActorItr(world); ActorItr; ++ActorItr)
	{
		if ((*ActorItr)->GetName() == "WorldViewCamera")
		{
			WorldViewCamera = *ActorItr;
		}
		if ((*ActorItr)->GetName() == "WorldViewCamFirst")
		{
			WorldViewCamFirst = *ActorItr;
		}
		if ((*ActorItr)->GetName() == "WorldViewCamSecond")
		{
			WorldViewCamSecond = *ActorItr;
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

}

/** APioneer ��ü�� �����մϴ�. */
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

/** �ٸ� ������ �����ϴ� �Լ��Դϴ�. */
void APioneerManager::SwitchPawn(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
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
	
	//// ���� WorldViewCam�� ī�޶� �����մϴ�.
	//if (PioneerCtrl && WorldViewCam)
	//	PioneerCtrl->SetViewTargetWithBlend(WorldViewCam, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	//else
	//	UE_LOG(LogTemp, Warning, TEXT("!(PioneerCtrl && WorldViewCam)"));

	// ���� First ī�޶� �Ű��ֱ�
	if (PioneerCtrl)
	{
		if (PioneerCtrl->GetPawn())
		{
			FVector location = PioneerCtrl->GetPawn()->GetActorLocation();
			location.Z = 5000.0f;
			WorldViewCamFirst->SetActorLocation(location);
			SwitchViewTarget(WorldViewCamFirst, BlendTime);
		}
	}

	// AI�� Pioneer�� ã���ϴ�.
	APioneer* Pawn = nullptr;
	for (auto& pioneer : Pioneers)
	{
		if (pioneer->SocketID == -1 && pioneer->bDead == false)
		{
			Pawn = pioneer;
			break;
		}
	}

	float OneFrameGap = 0.033f;

	// �״��� Second ī�޶� �Ű��ֱ�
	if (Pawn)
	{
		FVector location = Pawn->GetActorLocation();
		location.Z = 5000.0f;
		WorldViewCamSecond->SetActorLocation(location);

		FTimerHandle timer;
		FTimerDelegate timerDel;
		// �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		timerDel.BindUFunction(this, FName("SwitchViewTarget"), WorldViewCamSecond, BlendTime, blendFunc, BlendExp, bLockOutgoing);
		GetWorldTimerManager().SetTimer(timer, timerDel, BlendTime + OneFrameGap, false);

		FTimerHandle timer1;
		FTimerDelegate timerDel1;
		// �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		timerDel1.BindUFunction(this, FName("SwitchViewTarget"), Pawn, BlendTime, blendFunc, BlendExp, bLockOutgoing);
		GetWorldTimerManager().SetTimer(timer1, timerDel1, BlendTime * 2.0f + OneFrameGap, false);

		FTimerHandle timer2;
		FTimerDelegate timerDel2;
		// �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		timerDel2.BindUFunction(this, FName("PossessPioneer"), Pawn, BlendTime, blendFunc, BlendExp, bLockOutgoing);
		GetWorldTimerManager().SetTimer(timer2, timerDel2, BlendTime * 3.0f + OneFrameGap, false);
	}
	else
	{
		FTimerHandle timer;
		FTimerDelegate timerDel;
		// �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		timerDel.BindUFunction(this, FName("SwitchViewTarget"), WorldViewCamera, BlendTime, blendFunc, BlendExp, bLockOutgoing);
		GetWorldTimerManager().SetTimer(timer, timerDel, BlendTime + OneFrameGap, false);
	}
}

/** �ٸ� ���� ī�޶�� �����ϴ� �Լ��Դϴ�. */
void APioneerManager::SwitchViewTarget(AActor* Actor, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchViewTarget: !Actor"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("SwitchViewTarget: %s, BlendTime: %f"), *Actor->GetName(), BlendTime);
	PioneerCtrl->SetViewTargetWithBlend(Actor, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	
}

/** �ٸ� ���� Possess() �մϴ�. */
void APioneerManager::PossessPioneer(APioneer* Pioneer)
{
	if (!Pioneer)
	{
		for (auto& pioneer : Pioneers)
		{
			if (pioneer->SocketID == -1 && pioneer->bDead == false)
			{
				Pioneer = pioneer;
				break;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::PossessPioneer => !Pioneer"));
	}



	// PioneerCtrl�� �����ϴ��� Ȯ���մϴ�.
	if (!PioneerCtrl)
	{
		UE_LOG(LogTemp, Warning, TEXT("!PioneerCtrl"));
			return;
	}

	// PioneerCtrl�� Pawn�� �����ϰ� ������ ���� �����մϴ�.
	if (PioneerCtrl->GetPawn())
	{
		APioneer* ToDestroy = Cast<APioneer>(PioneerCtrl->GetPawn());

		PioneerCtrl->UnPossess();

		if (ToDestroy->bDead)
			ToDestroy->Destroy();

	}

	// �������� PioneerCtrl�� TmapPioneers[ID]�� �����մϴ�.
	PioneerCtrl->Possess(Pioneer);
	playerPioneer = Pioneer;
}