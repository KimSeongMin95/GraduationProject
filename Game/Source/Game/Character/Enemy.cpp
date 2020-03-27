// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Controller/EnemyAIController.h"

#include "Character/Pioneer.h"
#include "Building/Building.h"

#include "Network/Packet.h"
#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "EnemyManager.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AEnemy::AEnemy()
{
	InitFSM();

	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;

	ID = 0;

	EnemyManager = nullptr;

	EnemyType = EEnemyType::None;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAIController();

	// Init()이 끝나고 AIController에 빙의합니다.
	PossessAIController();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();

	if (!GetOwner())
	{
		// 게임클라이언트라면 게임서버에서 SpawnEnemy으로 생성하기 때문에 소멸시킵니다.
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
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	RotateTargetRotation(DeltaTime);
}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AEnemy::InitHelthPointBar()
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AEnemy::InitStat()
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void AEnemy::InitRanges()
{
	if (!DetectRangeSphereComp || !AttackRangeSphereComp)
		return;

	DetectRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_DetectRange);
	DetectRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_DetectRange);

	AttackRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_AttackRange);
	AttackRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_AttackRange);

	DetectRangeSphereComp->SetSphereRadius(AOnlineGameMode::CellSize * DetectRange);
	AttackRangeSphereComp->SetSphereRadius(AOnlineGameMode::CellSize * AttackRange);
}

void AEnemy::InitAIController()
{
	Super::InitAIController();

	// 이미 AIController를 가지고 있으면 생성하지 않음.
	if (AIController)
		return;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = GetTransform(); // 현재 PioneerManager 객체 위치를 기반으로 합니다.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AIController = World->SpawnActor<AEnemyAIController>(AEnemyAIController::StaticClass(), myTrans, SpawnParams);
}

void AEnemy::InitCharacterMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = AOnlineGameMode::CellSize * MoveSpeed; // 움직일 때 걷는 속도
}


void AEnemy::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// 자기 자신과 충돌하면 무시합니다.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// 만약 OtherActor가 APioneer이기는 하지만 APioneer의 DetectRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->GetDetectRangeSphereComp() == OtherComp || pioneer->GetAttackRangeSphereComp() == OtherComp)
				return;
		}

		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			// 만약 OtherActor가 ABuilding이기는 하지만 ABuilding이 건설할 수 있는지 확인하는 상태라면 존재하지 않으므로 무시.
			if (building->BuildingState == EBuildingState::Constructable)
				return;
		}

		//if (OverapedDetectRangeActors.Contains(OtherActor) == false)
		{
			OverapedDetectRangeActors.Add(OtherActor);
			//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Add(OtherActor): %s"), *OtherActor->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Num(): %d"), OverapedDetectRangeActors.Num());
			//UE_LOG(LogTemp, Warning, TEXT("_______"));
		}
	}
}
void AEnemy::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// 만약 OtherActor가 APioneer이기는 하지만 APioneer의 DetectRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->GetDetectRangeSphereComp() == OtherComp || pioneer->GetAttackRangeSphereComp() == OtherComp)
				return;
		}

		//OverapedDetectRangeActors.Remove(OtherActor); // OtherActor 전체를 지웁니다.
		OverapedDetectRangeActors.RemoveSingle(OtherActor); // OtherActor 하나를 지웁니다.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Num(): %d"), OverapedDetectRangeActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}

void AEnemy::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// 자기 자신과 충돌하면 무시합니다.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// 만약 OtherActor가 APioneer이기는 하지만 APioneer의 DetectRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->GetDetectRangeSphereComp() == OtherComp || pioneer->GetAttackRangeSphereComp() == OtherComp)
				return;
		}

		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			// 만약 OtherActor가 ABuilding이기는 하지만 ABuilding이 건설할 수 있는지 확인하는 상태라면 존재하지 않으므로 무시.
			if (building->BuildingState == EBuildingState::Constructable)
				return;
		}

		//if (OverapedAttackRangeActors.Contains(OtherActor) == false)
		{
			OverapedAttackRangeActors.Add(OtherActor);
			//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Add(OtherActor): %s"), *OtherActor->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Num(): %d"), OverapedAttackRangeActors.Num());
			//UE_LOG(LogTemp, Warning, TEXT("_______"));
		}
	}
}
void AEnemy::OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// 만약 OtherActor가 APioneer이기는 하지만 APioneer의 DetectRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->GetDetectRangeSphereComp() == OtherComp || pioneer->GetAttackRangeSphereComp() == OtherComp)
				return;
		}

		//OverapedAttackRangeActors.Remove(OtherActor); // OtherActor 전체를 지웁니다.
		OverapedAttackRangeActors.RemoveSingle(OtherActor); // OtherActor 하나만 지웁니다.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Num(): %d"), OverapedAttackRangeActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}


void AEnemy::RotateTargetRotation(float DeltaTime)
{
	// 회전을 할 필요가 없으면 실행하지 않습니다.
	if (!bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}


void AEnemy::SetHealthPoint(float Delta)
{
	if (bDying)
		return;

	Super::SetHealthPoint(Delta);

	// 죽으면
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
				ServerSocketInGame->SendDestroyBuilding(ID);
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
		// 공격 애니메이션이 끝난 후 AnimationBlueprint에서 EventGraph로 Atteck을 Idle로 바꿔줌
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
	// USkeletalMeshComponent에 USkeletalMesh을 설정합니다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(ReferencePathOfMesh);
	if (skeletalMeshAsset.Succeeded())
	{
		// 각 Enemy의 BP_Animation을 가져오기. (주의할 점은 .BP_PioneerAnimation_C로 UAnimBluprint가 아닌 UClass를 불러옴으로써 바로 적용하는 것입니다.)
		FString referencePathOfBP_AnimInstance = ReferencePathOfBP_AnimInstance;
		UClass* animBP = LoadObject<UClass>(NULL, *referencePathOfBP_AnimInstance);
		if (!animBP)
		{
			UE_LOG(LogTemp, Warning, TEXT("!animBP"));
		}
		else
			GetMesh()->SetAnimInstanceClass(animBP);

		// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
		GetMesh()->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(Scale);
		GetMesh()->SetRelativeRotation(Rotation);
		GetMesh()->SetRelativeLocation(Location);
	}
}

void AEnemy::InitFSM()
{
	State = EEnemyFSM::Idle;
}


void AEnemy::FindTheTargetActor()
{
	TargetActor = nullptr;

	for (auto& actor : OverapedDetectRangeActors)
	{
		if (actor->IsA(APioneer::StaticClass())) // 개척자
		{
			// APioneer가 죽어있다면 skip
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

	// 개척자를 발견하면 끝냄
	if (TargetActor)
		return;

	for (auto& actor : OverapedDetectRangeActors)
	{
		if (actor->IsA(ABuilding::StaticClass())) // 건물
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
	else if (OverapedAttackRangeActors.Contains(TargetActor))
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
// 네트워크
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

	// 이동
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
