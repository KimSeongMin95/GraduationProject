// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Controller/EnemyAIController.h"

#include "Character/Pioneer.h"
#include "Building/Building.h"

#include "Network/Packet.h"
#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "EnemyManager.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AEnemy::AEnemy()
{
	InitFSM();

	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;

	ID = 0;

	EnemyManager = nullptr;

	EnemyType = EEnemyType::None;

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap); // Projectile
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAIController();

	// Init()�� ������ AIController�� �����մϴ�.
	PossessAIController();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();

	if (!GetOwner())
	{
		// ����Ŭ���̾�Ʈ��� ���Ӽ������� SpawnEnemy���� �����ϱ� ������ �Ҹ��ŵ�ϴ�.
		if (ClientSocketInGame)
		{
			if (ClientSocketInGame->IsClientSocketOn())
			{
				Destroy();
				return;
			}
		}
	}
}

void AEnemy::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	RotateTargetRotation(DeltaTime);
}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AEnemy::InitHelthPointBar()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AEnemy::InitStat()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void AEnemy::InitRanges()
{
	if (!DetectRangeSphereComp || !AttackRangeSphereComp)
		return;

	DetectRangeSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
	DetectRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_DetectRange);
	DetectRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_DetectRange);
	DetectRangeSphereComp->SetSphereRadius(AOnlineGameMode::CellSize * DetectRange);

	AttackRangeSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
	AttackRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_AttackRange);
	AttackRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_AttackRange);
	AttackRangeSphereComp->SetSphereRadius(AOnlineGameMode::CellSize * AttackRange);
}

void AEnemy::InitAIController()
{
	Super::InitAIController();

	// �̹� AIController�� ������ ������ �������� ����.
	if (AIController)
		return;

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AEnemy::InitAIController()> if (!world)"));
#endif
		return;
	}

	FTransform myTrans = GetTransform(); // ���� PioneerManager ��ü ��ġ�� ������� �մϴ�.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AIController = world->SpawnActor<AEnemyAIController>(AEnemyAIController::StaticClass(), myTrans, SpawnParams);
}

void AEnemy::InitCharacterMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = AOnlineGameMode::CellSize * MoveSpeed; // ������ �� �ȴ� �ӵ�
}


void AEnemy::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	UE_LOG(LogTemp, Log, TEXT("<AEnemy::OnOverlapBegin_DetectRange(...)> Character FName: %s"), *OtherActor->GetFName().ToString());
#endif

	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			if (pioneer->GetCapsuleComponent() == OtherComp)
			{
				OverlappedDetectRangeActors.Add(OtherActor);
			}
		}
	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			if (building->BuildingState == EBuildingState::Constructing ||
				building->BuildingState == EBuildingState::Constructed)
			{
				OverlappedDetectRangeActors.Add(OtherActor);
			}
		}
	}


//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	//UE_LOG(LogTemp, Log, TEXT("OverlappedDetectRangeActors.Add(OtherActor): %s"), *OtherActor->GetName());
	//UE_LOG(LogTemp, Log, TEXT("OverlappedDetectRangeActors.Num(): %d"), OverlappedDetectRangeActors.Num());
	//UE_LOG(LogTemp, Log, TEXT("_______"));
//#endif
}
void AEnemy::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			if (pioneer->GetCapsuleComponent() == OtherComp)
			{
				OverlappedDetectRangeActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
			}
		}

	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		OverlappedDetectRangeActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
	}


//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	//UE_LOG(LogTemp, Log, TEXT("OverlappedDetectRangeActors.RemoveSingle(OtherActor): %s"), *OtherActor->GetName());
	//UE_LOG(LogTemp, Log, TEXT("OverlappedDetectRangeActors.Num(): %d"), OverlappedDetectRangeActors.Num());
	//UE_LOG(LogTemp, Log, TEXT("_______"));
//#endif
}

void AEnemy::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	UE_LOG(LogTemp, Log, TEXT("<AEnemy::OnOverlapBegin_AttackRange(...)> Character FName: %s"), *OtherActor->GetFName().ToString());
#endif

	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			if (pioneer->GetCapsuleComponent() == OtherComp)
			{
				if (!OverlappedAttackRangeActors.Contains(OtherActor))
					OverlappedAttackRangeActors.Add(OtherActor);
			}
		}
	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			if (building->BuildingState == EBuildingState::Constructing ||
				building->BuildingState == EBuildingState::Constructed)
			{
				if (!OverlappedAttackRangeActors.Contains(OtherActor))
					OverlappedAttackRangeActors.Add(OtherActor);
			}
		}
	}


//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			//UE_LOG(LogTemp, Log, TEXT("OverlappedAttackRangeActors.Add(OtherActor): %s"), *OtherActor->GetName());
			//UE_LOG(LogTemp, Log, TEXT("OverlappedAttackRangeActors.Num(): %d"), OverlappedAttackRangeActors.Num());
			//UE_LOG(LogTemp, Log, TEXT("_______"));
//#endif
	
}
void AEnemy::OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			if (pioneer->GetCapsuleComponent() == OtherComp)
			{
				OverlappedAttackRangeActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
			}
		}

	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		OverlappedAttackRangeActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
	}


//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	//UE_LOG(LogTemp, Log, TEXT("OverlappedAttackRangeActors.RemoveSingle(OtherActor): %s"), *OtherActor->GetName());
	//UE_LOG(LogTemp, Log, TEXT("OverlappedAttackRangeActors.Num(): %d"), OverlappedAttackRangeActors.Num());
	//UE_LOG(LogTemp, Log, TEXT("_______"));
//#endif
}


void AEnemy::RotateTargetRotation(float DeltaTime)
{
	// ȸ���� �� �ʿ䰡 ������ �������� �ʽ��ϴ�.
	if (!bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}


void AEnemy::SetHealthPoint(float Delta)
{
	if (bDying)
		return;

	Super::SetHealthPoint(Delta);

	// ������
	if (bDying)
	{
		if (EnemyManager)
		{
			if (EnemyManager->Enemies.Contains(ID))
			{
				EnemyManager->Enemies.Remove(ID);
			}
		}

		if (ServerSocketInGame)
		{
			if (ServerSocketInGame->IsServerOn())
			{
				ServerSocketInGame->SendDestroyEnemy(ID);
			}
		}
	}
}


void AEnemy::PossessAIController()
{
	ABaseCharacter::PossessAIController();


}


void AEnemy::RunFSM()
{
	switch (State)
	{
	case EEnemyFSM::Idle:
	{
		IdlingOfFSM();
		break;
	}
	case EEnemyFSM::Tracing:
	{
		TracingOfFSM();
		break;
	}
	case EEnemyFSM::Attack:
	{
		// ���� �ִϸ��̼��� ���� �� AnimationBlueprint���� EventGraph�� Atteck�� Idle�� �ٲ���
		AttackingOfFSM();
		break;
	}
	}
}

void AEnemy::RunBehaviorTree()
{
	Super::RunBehaviorTree();

}
/*** ABaseCharacter : End ***/


/*** AEnemy : Start ***/
void AEnemy::InitSkeletalAnimation(const TCHAR* ReferencePathOfMesh, const FString ReferencePathOfBP_AnimInstance,
	FVector Scale /*= FVector::ZeroVector*/, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Location /*= FVector::ZeroVector*/)
{
	// USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(ReferencePathOfMesh);
	if (skeletalMeshAsset.Succeeded())
	{
		// �� Enemy�� BP_Animation�� ��������. (������ ���� .BP_PioneerAnimation_C�� UAnimBluprint�� �ƴ� UClass�� �ҷ������ν� �ٷ� �����ϴ� ���Դϴ�.)
		FString referencePathOfBP_AnimInstance = ReferencePathOfBP_AnimInstance;
		UClass* animBP = LoadObject<UClass>(NULL, *referencePathOfBP_AnimInstance);
		if (!animBP)
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<AEnemy::InitSkeletalAnimation(...)> if (!animBP)"));
#endif
		}
		else
			GetMesh()->SetAnimInstanceClass(animBP);

		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(Scale);
		GetMesh()->SetRelativeRotation(Rotation);
		GetMesh()->SetRelativeLocation(Location);

		GetMesh()->SetGenerateOverlapEvents(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}

void AEnemy::InitFSM()
{
	State = EEnemyFSM::Idle;
}


void AEnemy::FindTheTargetActor()
{
	TargetActor = nullptr;

	for (auto& actor : OverlappedDetectRangeActors)
	{
		if (actor->IsA(APioneer::StaticClass())) // ��ô��
		{
			// APioneer�� �׾��ִٸ� skip
			if (APioneer* pioneer = Cast<APioneer>(actor))
			{
				if (pioneer->bDying)
					continue;
			}

			if (!TargetActor)
			{
				TargetActor = actor;
				continue;
			}

			if (DistanceToActor(actor) < DistanceToActor(TargetActor))
				TargetActor = actor;
		}
	}

	// ��ô�ڸ� �߰��ϸ� ����
	if (TargetActor)
		return;

	for (auto& actor : OverlappedDetectRangeActors)
	{
		if (actor->IsA(ABuilding::StaticClass())) // �ǹ�
		{
			if (!TargetActor)
			{
				TargetActor = actor;
				continue;
			}

			if (DistanceToActor(actor) < DistanceToActor(TargetActor))
				TargetActor = actor;
		}
	}
}

void AEnemy::IdlingOfFSM()
{
	FindTheTargetActor();

	if (TargetActor)
		State = EEnemyFSM::Tracing;
}

void AEnemy::TracingOfFSM()
{
	FindTheTargetActor();

	TracingTargetActor();

	if (!TargetActor)
	{
		State = EEnemyFSM::Idle;
		if (AIController)
			AIController->StopMovement();
	}
	else if (OverlappedAttackRangeActors.Contains(TargetActor))
	{
		State = EEnemyFSM::Attack;
		if (AIController)
			AIController->StopMovement();
	}
}

void AEnemy::AttackingOfFSM()
{

}


void AEnemy::DamageToTargetActor()
{
	if (!TargetActor)
		return;

	if (TargetActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(TargetActor))
		{
			pioneer->SetHealthPoint(-AttackPower);

			if (pioneer->bDying)
				TargetActor = nullptr;

			return;
		}
	}

	if (TargetActor->IsA(ABuilding::StaticClass()))
	{
		if (ABuilding* building = dynamic_cast<ABuilding*>(TargetActor))
		{
			building->SetHealthPoint(-AttackPower);

			if (!building)
				TargetActor = nullptr;
			else if (building->BuildingState == EBuildingState::Destroying)
				TargetActor = nullptr;

			return;
		}
	}
}



///////////
// ��Ʈ��ũ
///////////
void AEnemy::SetInfoOfEnemy_Spawn(class cInfoOfEnemy_Spawn& Spawn)
{
	ID = Spawn.ID;

	EnemyType = (EEnemyType)Spawn.EnemyType;
}
class cInfoOfEnemy_Spawn AEnemy::GetInfoOfEnemy_Spawn()
{
	cInfoOfEnemy_Spawn spawn;

	spawn.ID = ID;

	spawn.EnemyType = (int)EnemyType;

	return spawn;
}

void AEnemy::SetInfoOfEnemy_Animation(class cInfoOfEnemy_Animation& Animation)
{
	SetActorTransform(Animation.GetActorTransform());

	TargetRotation = FRotator(Animation.TargetRotX, Animation.TargetRotY, Animation.TargetRotZ);

	// �̵�
	if (UCharacterMovementComponent* characterMovement = GetCharacterMovement())
		characterMovement->Velocity = FVector(Animation.VelocityX, Animation.VelocityY, Animation.VelocityZ);

	State = (EEnemyFSM)Animation.State;
}
class cInfoOfEnemy_Animation AEnemy::GetInfoOfEnemy_Animation()
{
	cInfoOfEnemy_Animation animation;

	animation.ID = ID;

	animation.SetActorTransform(GetActorTransform());

	animation.TargetRotX = TargetRotation.Pitch;
	animation.TargetRotY = TargetRotation.Yaw;
	animation.TargetRotZ = TargetRotation.Roll;

	FVector velocity = GetVelocity();
	animation.VelocityX = velocity.X;
	animation.VelocityY = velocity.Y;
	animation.VelocityZ = velocity.Z;

	animation.State = (int)State;

	return animation;
}

void AEnemy::SetInfoOfEnemy_Stat(class cInfoOfEnemy_Stat& Stat)
{
	HealthPoint = Stat.HealthPoint;
	SetHealthPoint(NULL);

	MaxHealthPoint = Stat.MaxHealthPoint;

	MoveSpeed = Stat.MoveSpeed;
	AttackSpeed = Stat.AttackSpeed;

	AttackPower = Stat.AttackPower;

	SightRange = Stat.SightRange;
	DetectRange = Stat.DetectRange;
	AttackRange = Stat.AttackRange;
}
class cInfoOfEnemy_Stat AEnemy::GetInfoOfEnemy_Stat()
{
	cInfoOfEnemy_Stat stat;

	stat.ID = ID;

	stat.HealthPoint = HealthPoint;
	stat.MaxHealthPoint = MaxHealthPoint;

	stat.MoveSpeed = MoveSpeed;
	stat.AttackSpeed = AttackSpeed;

	stat.AttackPower = AttackPower;

	stat.SightRange = SightRange;
	stat.DetectRange = DetectRange;
	stat.AttackRange = AttackRange;

	return stat;
}

void AEnemy::SetInfoOfEnemy(class cInfoOfEnemy& InfoOfEnemy)
{
	SetInfoOfEnemy_Spawn(InfoOfEnemy.Spawn);
	SetInfoOfEnemy_Animation(InfoOfEnemy.Animation);
	SetInfoOfEnemy_Stat(InfoOfEnemy.Stat);
}
class cInfoOfEnemy AEnemy::GetInfoOfEnemy()
{
	cInfoOfEnemy infoOfEnemy(ID, GetInfoOfEnemy_Spawn(), GetInfoOfEnemy_Animation(), GetInfoOfEnemy_Stat());

	return infoOfEnemy;
}
/*** AEnemy : End ***/
