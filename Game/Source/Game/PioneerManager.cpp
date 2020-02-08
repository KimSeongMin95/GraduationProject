// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Etc/WorldViewCameraActor.h"
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"
/*** ���� ������ ��� ���� ���� : End ***/


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
	
	// UWorld���� APioneerController�� ã���ϴ�.
	if (!PioneerCtrl)
	{
		for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
		{
			PioneerCtrl = *ActorItr;
		}
	}

	// UWorld���� APioneer�� ã�� TArray�� �߰��մϴ�.
	for (TActorIterator<APioneer> ActorItr(world); ActorItr; ++ActorItr)
	{
		if (Pioneers.Contains(*ActorItr) == false) // �̹� �߰��Ǿ����� �ʴٸ�
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
			// ��ġ ����
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
	
	/* SwitchPawn ����
	1. ��� CurrentPioneer�� ī�޶󿡼� CameraOfCurrentPioneer�� ��ȯ
	2. SwitchTime ���� CameraOfCurrentPioneer���� WorldViewCameraOfCurrentPioneer�� ��ȯ
	3. ��ȯ��Ű�⿡ ������ Pioneer Ž��
	4. SwitchTime ���� WorldViewCameraOfCurrentPioneer���� WorldViewCameraOfNextPioneer�� ��ȯ
	5. Ž���� Pioneer�� ����ִٸ� SwitchTime ���� WorldViewCameraOfNextPioneer���� CameraOfNextPioneer�� ��ȯ
	6. ��ȯ���� Pioneer�� ������ �ٽ� 3������ ȸ��
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
		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchPawn: else"));

		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
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


	// ��ȯ�� APioneer�� ã����
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
		timerDel.BindUFunction(this, FName("SwitchNext"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
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

			// FindTargetViewActor�� WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer ��ȯ ������ �ذ��ϱ� ����,
			// WorldViewCameraOfNextPioneer�� ��ġ�� WorldViewCameraOfCurrentPioneer�� �����ϰ� ���� WorldViewCameraOfCurrentPioneer�� ��ȯ
			WorldViewCameraOfCurrentPioneer->SetActorTransform(WorldViewCameraOfNextPioneer->GetActorTransform());
			PioneerCtrl->SetViewTarget(WorldViewCameraOfCurrentPioneer);

			FTimerDelegate timerDel;
			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
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
	timerDel.BindUFunction(this, FName("SwitchFinish"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
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
			// FindTargetViewActor�� WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer ��ȯ ������ �ذ��ϱ� ����,
			// WorldViewCameraOfNextPioneer�� ��ġ�� WorldViewCameraOfCurrentPioneer�� �����ϰ� ���� WorldViewCameraOfCurrentPioneer�� ��ȯ
			WorldViewCameraOfCurrentPioneer->SetActorTransform(WorldViewCameraOfNextPioneer->GetActorTransform());
			SwitchViewTarget(WorldViewCameraOfCurrentPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);

			UE_LOG(LogTemp, Warning, TEXT("APioneerManager::SwitchFinish: if (pioneer->bDying || pioneer->IsActorBeingDestroyed())"));

			FTimerDelegate timerDel;
			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
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

	// �÷��̾ �����ϴ� ��ô�ڸ� �����մϴ�.
	PioneerOfPlayer = Cast<APioneer>(TargetViewActor);

	// TargetViewActor�� �ٽ� �ʱ�ȭ�մϴ�.
	TargetViewActor = nullptr;

	// PioneerCtrl�� �����ϴ��� Ȯ���մϴ�.
	if (!PioneerCtrl)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneerManager::PossessPioneer: !PioneerCtrl"));
			return;
	}

	// PioneerCtrl�� Pawn�� �����ϰ� ������ �Ҹ��ŵ�ϴ�.
	if (PioneerCtrl->GetPawn())
		PioneerCtrl->GetPawn()->Destroy();

	PioneerCtrl->Possess(Pioneer);

	UE_LOG(LogTemp, Warning, TEXT("APioneerManager::PossessPioneer: PioneerCtrl->Possess(Pioneer);"));
}
/*** APioneerManager : End ***/
