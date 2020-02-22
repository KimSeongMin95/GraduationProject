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

	// ī�޶���� �����մϴ�.
	SpawnWorldViewCameraActor(&WorldViewCamera, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 20000.0f)));
	SpawnWorldViewCameraActor(&CameraOfCurrentPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 5000.0f)));
	SpawnWorldViewCameraActor(&WorldViewCameraOfCurrentPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 5000.0f)));
	SpawnWorldViewCameraActor(&WorldViewCameraOfNextPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 5000.0f)));
	SpawnWorldViewCameraActor(&CameraOfNextPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 5000.0f)));

	// UWorld���� APioneer�� ã�� TArray�� �߰��մϴ�.
	FindPioneersInWorld();
}

void APioneerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SwitchTick();
}
/*** Basic Function : End ***/


/*** APioneerManager : Start ***/
void APioneerManager::SpawnWorldViewCameraActor(class AWorldViewCameraActor** WorldViewCameraActor, FTransform Transform)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		printf_s("[ERROR] <APioneerManager::SpawnWorldViewCameraActor(...)> if (!World)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SpawnWorldViewCameraActor(...)> if (!World)"));
		return;
	}

	FTransform myTrans = Transform;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name�� �����մϴ�. World Outliner�� ǥ��Ǵ� Label���� �ٸ��ϴ�.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	*WorldViewCameraActor = World->SpawnActor<AWorldViewCameraActor>(AWorldViewCameraActor::StaticClass(), myTrans, SpawnParams);

	// ��Ⱦ�� ������ �����ν� ȭ���� ������ ������ �����մϴ�.
	if (*WorldViewCameraActor)
		(*WorldViewCameraActor)->GetCameraComponent()->bConstrainAspectRatio = false;
}

void APioneerManager::FindPioneersInWorld()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <APioneerManager::FindPioneersInWorld()> if (!world)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::FindPioneersInWorld()> if (!world)"));
		return;
	}

	for (TActorIterator<APioneer> ActorItr(world); ActorItr; ++ActorItr)
	{
		if (Pioneers.Contains(*ActorItr) == false) // �̹� �߰��Ǿ����� �ʴٸ�
			Pioneers.Add(*ActorItr);

		ActorItr->SetPioneerManager(this);
	}
}

/////////////////////////////////////////
// ViewTarget�� Possess ��ȯ
/////////////////////////////////////////
void APioneerManager::SwitchViewTarget(AActor* Actor, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	if (!Actor)
	{
		printf_s("[ERROR] <APioneerManager::SwitchViewTarget(...)> if (!Actor)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchViewTarget(...)> if (!Actor)"));
		return;
	}
	
	if (!PioneerController)
	{
		printf_s("[ERROR] <APioneerManager::SwitchViewTarget(...)> if (!PioneerController)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchViewTarget(...)> if (!PioneerController)"));
		return;
	}
	
	PioneerController->SetViewTargetWithBlend(Actor, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	printf_s("[INFO] <APioneerManager::SwitchViewTarget(...)> %s, BlendTime: %f\n", TCHAR_TO_ANSI(*Actor->GetName()), BlendTime);
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchViewTarget(...)> %s, BlendTime: %f"), *Actor->GetName(), BlendTime);
}

void APioneerManager::FindTargetViewActor(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	printf_s("[INFO] <APioneerManager::FindTargetViewActor(...)>\n");
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::FindTargetViewActor(...)>"));
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
		printf_s("[INFO] <APioneerManager::FindTargetViewActor(...)> if (TargetViewActor)\n");
		UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::FindTargetViewActor(...)> if (TargetViewActor)"));

		FVector location = TargetViewActor->GetActorLocation();
		location.Z = 5000.0f;

		if (WorldViewCameraOfNextPioneer)
		{
			WorldViewCameraOfNextPioneer->SetActorLocation(location);
			SwitchViewTarget(WorldViewCameraOfNextPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
		}
		else
		{
			printf_s("[ERROR] <APioneerManager::FindTargetViewActor(...)> if (!WorldViewCameraOfNextPioneer)\n");
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::FindTargetViewActor(...)> if (!WorldViewCameraOfNextPioneer)"));
		}

		GetWorldTimerManager().ClearTimer(TimerHandleOfFindTargetViewActor);

		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("SwitchNext"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		GetWorldTimerManager().SetTimer(TimerOfSwitchNext, timerDel, 0.1f, false, BlendTime + 0.5f);
	}
}

void APioneerManager::SwitchNext(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	printf_s("[INFO] <APioneerManager::SwitchNext(...)>\n");
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchNext(...)>"));

	SwitchState = ESwitchState::Next;

	if (!TargetViewActor)
	{
		SwitchState = ESwitchState::Switchable; 

		printf_s("[ERROR] <APioneerManager::SwitchNext(...)> if (!TargetViewActor)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchNext(...)> if (!TargetViewActor)"));
		return;
	}

	if (APioneer* pioneer = Cast<APioneer>(TargetViewActor))
	{
		if (pioneer->bDying || pioneer->IsActorBeingDestroyed())
		{
			printf_s("[INFO] <APioneerManager::SwitchNext(...)> if (pioneer->bDying || pioneer->IsActorBeingDestroyed())\n");
			UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchNext(...)> if (pioneer->bDying || pioneer->IsActorBeingDestroyed())"));

			// FindTargetViewActor�� WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer ��ȯ ������ �ذ��ϱ� ����,
			// WorldViewCameraOfNextPioneer�� ��ġ�� WorldViewCameraOfCurrentPioneer�� �����ϰ� ���� WorldViewCameraOfCurrentPioneer�� ��ȯ
			if (WorldViewCameraOfCurrentPioneer && WorldViewCameraOfNextPioneer)
			{
				WorldViewCameraOfCurrentPioneer->SetActorTransform(WorldViewCameraOfNextPioneer->GetActorTransform());

				if (PioneerController)
					PioneerController->SetViewTarget(WorldViewCameraOfCurrentPioneer);
				else
				{
					printf_s("[ERROR] <APioneerManager::SwitchNext(...)> if (WorldViewCameraOfCurrentPioneer && WorldViewCameraOfNextPioneer) && if (!PioneerController)\n");
					UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchNext(...)> if (WorldViewCameraOfCurrentPioneer && WorldViewCameraOfNextPioneer) && if (!PioneerController)"));
				}
			}
			else
			{
				printf_s("[ERROR] <APioneerManager::SwitchNext(...)> if (!WorldViewCameraOfCurrentPioneer || !WorldViewCameraOfNextPioneer)\n");
				UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchNext(...)> if (!WorldViewCameraOfCurrentPioneer || !WorldViewCameraOfNextPioneer)"));
			}

			FTimerDelegate timerDel;
			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
			GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true);

			return;
		}

		pioneer->CopyTopDownCameraTo(CameraOfCurrentPioneer);
	}

	if (!PioneerController)
	{
		printf_s("[ERROR] <APioneerManager::SwitchNext(...)> if (!PioneerController)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchNext(...)> if (!PioneerController)"));
		return;
	}

	PioneerController->SetViewTargetWithBlend(CameraOfNextPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
	printf_s("[INFO] <APioneerManager::SwitchNext(...)> %s, BlendTime: %f\n", TCHAR_TO_ANSI(*CameraOfNextPioneer->GetName()), BlendTime);
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchNext(...)> %s, BlendTime: %f"), *CameraOfNextPioneer->GetName(), BlendTime);

	FTimerDelegate timerDel;
	timerDel.BindUFunction(this, FName("SwitchFinish"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
	GetWorldTimerManager().SetTimer(TimerHandleOfSwitchFinish, timerDel, 0.1f, false, BlendTime + 0.25f);
}

void APioneerManager::SwitchFinish(float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
{
	printf_s("[INFO] <APioneerManager::SwitchFinish(...)>\n");
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchFinish(...)>"));

	SwitchState = ESwitchState::Finish;

	if (!TargetViewActor)
	{
		SwitchState = ESwitchState::Switchable;

		printf_s("[ERROR] <APioneerManager::SwitchFinish(...)> if (!TargetViewActor)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchFinish(...)> if (!TargetViewActor)"));
		return;
	}
	
	if (APioneer* pioneer = Cast<APioneer>(TargetViewActor))
	{
		if (pioneer->bDying || pioneer->IsActorBeingDestroyed())
		{
			// FindTargetViewActor�� WorldViewCameraOfNextPioneer -> WorldViewCameraOfNextPioneer ��ȯ ������ �ذ��ϱ� ����,
			// WorldViewCameraOfNextPioneer�� ��ġ�� WorldViewCameraOfCurrentPioneer�� �����ϰ� ���� WorldViewCameraOfCurrentPioneer�� ��ȯ
			if (WorldViewCameraOfCurrentPioneer && WorldViewCameraOfNextPioneer)
			{
				WorldViewCameraOfCurrentPioneer->SetActorTransform(WorldViewCameraOfNextPioneer->GetActorTransform());
				SwitchViewTarget(WorldViewCameraOfCurrentPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
			}
			else
			{
				printf_s("[ERROR] <APioneerManager::SwitchFinish(...)> if (!WorldViewCameraOfCurrentPioneer || !WorldViewCameraOfNextPioneer)\n");
				UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchFinish(...)> if (!WorldViewCameraOfCurrentPioneer || !WorldViewCameraOfNextPioneer)"));
			}
			printf_s("[INFO] <APioneerManager::SwitchFinish(...)> if (pioneer->bDying || pioneer->IsActorBeingDestroyed())\n");
			UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchFinish(...)> if (pioneer->bDying || pioneer->IsActorBeingDestroyed())"));

			FTimerDelegate timerDel;
			timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
			GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);

			return;
		}

		PossessPioneer(Cast<APioneer>(TargetViewActor));
		SwitchState = ESwitchState::Switchable;
	}

}

void APioneerManager::PossessPioneer(class APioneer* Pioneer)
{
	printf_s("[INFO] <APioneerManager::PossessPioneer(...)>\n");
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::PossessPioneer(...)>"));

	// �÷��̾ �����ϴ� ��ô�ڸ� �����մϴ�.
	PioneerOfPlayer = Cast<APioneer>(TargetViewActor);

	// TargetViewActor�� �ٽ� �ʱ�ȭ�մϴ�.
	TargetViewActor = nullptr;

	// PioneerController�� �����ϴ��� Ȯ���մϴ�.
	if (!PioneerController)
	{
		printf_s("[ERROR] <APioneerManager::PossessPioneer(...)> if (!PioneerController)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::PossessPioneer(...)> if (!PioneerController)"));
		return;
	}

	// PioneerController�� Pawn�� �����ϰ� ������ �Ҹ��ŵ�ϴ�.
	if (PioneerController->GetPawn())
		PioneerController->GetPawn()->Destroy();

	PioneerController->Possess(Pioneer);

	printf_s("[INFO] <APioneerManager::PossessPioneer(...)> PioneerController->Possess(Pioneer);\n");
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::PossessPioneer(...)> PioneerController->Possess(Pioneer);"));
}

void APioneerManager::SwitchTick()
{
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


/////////////////////////////////////////
// public
/////////////////////////////////////////
void APioneerManager::SetPioneerController(class APioneerController* pPioneerController)
{
	printf_s("[INFO] <APioneerManager::SetPioneerController()>\n");
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SetPioneerController()>"));

	this->PioneerController = pPioneerController;
}

APioneer* APioneerManager::GetPioneerBySocketID(int SocketID)
{
	for (auto& pioneer : Pioneers)
	{
		if (!pioneer)
			continue;

		if (pioneer->SocketID == SocketID)
			return pioneer;
	}

	return nullptr;
}

void APioneerManager::SpawnPioneer(FTransform Transform)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		printf_s("[ERROR] <APioneerManager::SpawnPioneer(...)> if (!World)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SpawnPioneer(...)> if (!World)"));
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

	APioneer* pioneer = World->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);

	if (!pioneer)
	{
		printf_s("[ERROR] <APioneerManager::SpawnPioneer(...)> if (!pioneer)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SpawnPioneer(...)> if (!pioneer)"));
		return;
	}

	pioneer->SetPioneerManager(this);
	Pioneers.Add(pioneer);
}

void APioneerManager::SwitchOtherPioneer(class APioneer* CurrentPioneer, float BlendTime, EViewTargetBlendFunction blendFunc, float BlendExp, bool bLockOutgoing)
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
		printf_s("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (SwitchState != ESwitchState::Switchable)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (SwitchState != ESwitchState::Switchable)"));
		return;
	}

	if (!PioneerController)
	{
		printf_s("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (!PioneerController)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (!PioneerController)"));
		return;
	}

	if (CurrentPioneer)
	{
		if (CurrentPioneer->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)
			return;

		PioneerController->SetViewTargetWithBlend(CameraOfCurrentPioneer);

		FVector location = CurrentPioneer->GetActorLocation();
		location.Z = 5000.0f;

		if (WorldViewCameraOfCurrentPioneer)
		{
			WorldViewCameraOfCurrentPioneer->SetActorLocation(location);
			SwitchViewTarget(WorldViewCameraOfCurrentPioneer, BlendTime, blendFunc, BlendExp, bLockOutgoing);
		}
		else
		{
			printf_s("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (!WorldViewCameraOfCurrentPioneer)\n");
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneerManager::SwitchOtherPioneer(...)> if (!WorldViewCameraOfCurrentPioneer)"));
		}

		SwitchState = ESwitchState::Current;

		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
	}
	else
	{
		printf_s("[INFO] <APioneerManager::SwitchOtherPioneer(...)> else\n");
		UE_LOG(LogTemp, Warning, TEXT("[INFO] <APioneerManager::SwitchOtherPioneer(...)> else"));

		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("FindTargetViewActor"), BlendTime, blendFunc, BlendExp, bLockOutgoing); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		GetWorldTimerManager().SetTimer(TimerHandleOfFindTargetViewActor, timerDel, 0.1f, true, BlendTime + 0.5f);
	}
}
/*** APioneerManager : End ***/






//for (TActorIterator<AWorldViewCameraActor> ActorItr(world); ActorItr; ++ActorItr)
//{
//	if ((*ActorItr)->GetName() == "WorldViewCamera")
//	{
//		WorldViewCamera = *ActorItr;
//	}
//
//	else if ((*ActorItr)->GetName() == "CameraOfCurrentPioneer")
//	{
//		CameraOfCurrentPioneer = *ActorItr;
//	}
//	else if ((*ActorItr)->GetName() == "WorldViewCameraOfCurrentPioneer")
//	{
//		WorldViewCameraOfCurrentPioneer = *ActorItr;
//	}
//
//	else if ((*ActorItr)->GetName() == "WorldViewCameraOfNextPioneer")
//	{
//		WorldViewCameraOfNextPioneer = *ActorItr;
//	}
//	else if ((*ActorItr)->GetName() == "CameraOfNextPioneer")
//	{
//		CameraOfNextPioneer = *ActorItr;
//	}
//}