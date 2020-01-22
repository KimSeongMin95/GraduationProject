// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "AnimInstance/PioneerAnimInstance.h"

#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"

#include "PioneerManager.h"

#include "Item/Weapon/AssaultRifle.h"
#include "Item/Weapon/GrenadeLauncher.h"
#include "Item/Weapon/Pistol.h"
#include "Item/Weapon/RocketLauncher.h"
#include "Item/Weapon/Shotgun.h"
#include "Item/Weapon/SniperRifle.h"

#include "Building/Wall.h"
#include "Building/Floor.h"
#include "Building/Stairs.h"
#include "Building/Turret.h"
#include "Building/Gate.h"
#include "Building/OrganicMine.h"
#include "Building/InorganicMine.h"
#include "Building/NuclearFusionPowerPlant.h"
#include "Building/ResearchInstitute.h"
#include "Building/WeaponFactory.h"

#include "Landscape.h"

#include "Character/Enemy.h"

#include "Item/Item.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
APioneer::APioneer()
{
	SocketID = -1; // -1�� AI�� ���մϴ�.
	InitPioneerManager();


	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	InitStat();

	InitRanges();

	InitHelthPointBar();

	InitCharacterMovement();

	InitSkeletalAnimation();
	
	InitCamera();

	InitCursor();

	InitEquipments();

	/*** Building : Start ***/
	bConstructingMode = false;
	/*** Building : End ***/

	InitFSM();

	InitItem();
	//// �Է� ó���� ���� ��ȸ���� �����մϴ�.
	//BaseTurnRate = 45.0f;
	//BaseLookUpRate = 45.0f;
}

void APioneer::BeginPlay()
{
	Super::BeginPlay();

	InitAIController();

	// Init()�� ������ AIController�� �����մϴ�.
	PossessAIController();

	InitWeapon();
}

void APioneer::Tick(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Test"));

	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	SetCursorToWorld();

	OnConstructingMode();

	RotateTargetRotation(DeltaTime);

	// ȸ���� ������ �����ϱ� ���� ī�޶� ��ġ ������ ���� �������� �����մϴ�.
	SetCameraBoomSettings();
}

void APioneer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up game play key bindings
	check(PlayerInputComponent);

}
/*** Basic Function : End ***/

/*** PioneerManager : Start ***/
void APioneer::InitPioneerManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::FindPioneerManager: !world"));
		return;
	}

	for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
	{
		PioneerManager = *ActorItr;
	}

	// �߰�
	if (PioneerManager)
	{
		if (PioneerManager->Pioneers.Contains(this) == false)
			PioneerManager->Pioneers.Add(this);
	}
}

void APioneer::DestroyCharacter()
{
	// Weapon�� PioneerController�� PioneerAIController ������� ������ ���� ���� �Ҹ�
	for (auto& weapon : Weapons)
	{
		if (weapon)
			weapon->Destroy();
	}



	// AIController�� �̹� ���ŵǾ����Ƿ� �÷��̾ �����ϴ� ��ô�ڰ� �ƴϸ� �ٷ� �Ҹ�
	if (!GetController())
	{
		Destroy();
		return;
	}

	if (GetMesh())
		GetMesh()->DestroyComponent();

	if (GetCharacterMovement())
		GetCharacterMovement()->DestroyComponent();

	if (HelmetMesh)
		HelmetMesh->DestroyComponent();

	if (PioneerManager)
	{
		PioneerManager->Pioneers.Remove(this);
		PioneerManager->SwitchOtherPioneer(this, 1.0f);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("APioneer::DestroyCharacter: if (PioneerManager) else"));


	// ���⼭ Destroy()�ϴ� ��ſ� PioneerManager�� PossessPioneer()���� Destroy�� ��� ��.
	//Destroy();
}

bool APioneer::CopyTopDownCameraTo(AActor* CameraToBeCopied)
{
	if (!TopDownCameraComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("!APioneer::CopyTopDownCameraToPioneerManager: !TopDownCameraComponent"));
		return false;
	}

	if (!CameraToBeCopied)
	{
		UE_LOG(LogTemp, Warning, TEXT("!APioneer::CopyTopDownCameraToPioneerManager: !CameraToBeCopied"));
		return false;
	}

	CameraToBeCopied->SetActorTransform(TopDownCameraComponent->GetComponentTransform());
	return true;
}
/*** PioneerManager : End ***/

/*** Stat : Start ***/
void APioneer::SetHealthPoint(float Delta)
{
	if (bDying)
		return;

	Super::SetHealthPoint(Delta);

	if (HealthPoint > 0.0f)
		return;

	if (CursorToWorld)
		CursorToWorld->DestroyComponent();

	if (Building)
		Building->Destroy();

	if (AIController)
	{
		AIController->UnPossess();
		AIController->Destroy();
	}
}

void APioneer::InitStat()
{
	HealthPoint = 100.0f;
	MaxHealthPoint = 100.0f;
	bDying = false;

	MoveSpeed = 10.0f;
	AttackSpeed = 1.0f;

	AttackPower = 1.0f;

	AttackRange = 16.0f;
	DetectRange = 32.0f;
	SightRange = 32.0f;
}

void APioneer::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// �ڱ� �ڽŰ� �浹�ϸ� �����մϴ�.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// ���� OtherActor�� AEnemy�̱�� ������ AEnemy�� DetactRangeSphereComp �Ǵ� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (enemy->DetactRangeSphereComp == OtherComp || enemy->AttackRangeSphereComp == OtherComp)
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
void APioneer::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// ���� OtherActor�� AEnemy�̱�� ������ AEnemy�� DetactRangeSphereComp �Ǵ� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (enemy->DetactRangeSphereComp == OtherComp || enemy->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedDetectRangeActors.Remove(OtherActor); // OtherActor ��ü�� ����ϴ�.
		OverapedDetectRangeActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Num(): %d"), OverapedDetectRangeActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}

void APioneer::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// �ڱ� �ڽŰ� �浹�ϸ� �����մϴ�.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// ���� OtherActor�� AEnemy�̱�� ������ AEnemy�� DetactRangeSphereComp �Ǵ� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (enemy->DetactRangeSphereComp == OtherComp || enemy->AttackRangeSphereComp == OtherComp)
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
void APioneer::OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// ���� OtherActor�� AEnemy�̱�� ������ AEnemy�� DetactRangeSphereComp �Ǵ� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (enemy->DetactRangeSphereComp == OtherComp || enemy->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedAttackRangeActors.Remove(OtherActor); // OtherActor ��ü�� ����ϴ�.
		OverapedAttackRangeActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Num(): %d"), OverapedAttackRangeActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}

void APioneer::InitRanges()
{
	if (!DetactRangeSphereComp || !AttackRangeSphereComp)
		return;

	DetactRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &APioneer::OnOverlapBegin_DetectRange);
	DetactRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &APioneer::OnOverlapEnd_DetectRange);

	AttackRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &APioneer::OnOverlapBegin_AttackRange);
	AttackRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &APioneer::OnOverlapEnd_AttackRange);

	DetactRangeSphereComp->SetSphereRadius(AMyGameModeBase::CellSize * DetectRange);
	AttackRangeSphereComp->SetSphereRadius(AMyGameModeBase::CellSize * AttackRange);
}
/*** Stat : End ***/

/*** IHealthPointBarInterface : Start ***/
void APioneer::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** IHealthPointBarInterface : End ***/

/*** CharacterMovement : Start ***/
void APioneer::InitCharacterMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = AMyGameModeBase::CellSize * MoveSpeed; // ������ �� �ȴ� �ӵ�
}

void APioneer::RotateTargetRotation(float DeltaTime)
{
	// ���Ⱑ ���ų� ȸ���� �� �ʿ䰡 ������ �������� �ʽ��ϴ�.
	if (!CurrentWeapon || !bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}

void APioneer::StopMovement()
{
	if (GetController())
		GetController()->StopMovement();
}
/*** CharacterMovement : End ***/

/*** SkeletalAnimation : Start ***/
void APioneer::InitSkeletalAnimation()
{
	// 1. USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Pioneer/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	}
	//// 2. Skeleton�� �����ɴϴ�.
	//static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Character/Mesh/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton'"));
	//if (skeleton.Succeeded())
	//{
	//	Skeleton = skeleton.Object;
	//}
	//// 3. PhysicsAsset�� �����ɴϴ�.
	//static ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(TEXT("PhysicsAsset'/Game/Character/Mesh/SK_Mannequin_PhysicsAsset.SK_Mannequin_PhysicsAsset'"));
	//if (physicsAsset.Succeeded())
	//{
	//	GetMesh()->SetPhysicsAsset(physicsAsset.Object);
	//}
	//// 4.1 AnimInstance�� �������� �ִϸ��̼� / ��Ÿ�ֿ� ��ȣ �ۿ��ϸ� �����ϴ� Ŭ������ �� �����ϴ�.
	//static ConstructorHelpers::FClassFinder<UPioneerAnimInstance> pioneerAnimInstance(TEXT("Class'/Script/Game.PioneerAnimInstance'"));
	//if (pioneerAnimInstance.Succeeded())
	//{
	//	GetMesh()->SetAnimInstanceClass(pioneerAnimInstance.Class);
	//}
	// 4.2 AnimInstance�� AnimationBlueprint�� AnimClass�ν� ���� ������ �մϴ�.
	// �ϴ� �������Ʈ�� ����ϰڽ��ϴ�. (������ ���� .BP_PioneerAnimation_C�� UAnimBluprint�� �ƴ� UClass�� �ҷ������ν� �ٷ� �����ϴ� ���Դϴ�.)
	FString animBP_Reference = "UClass'/Game/Characters/Pioneer/Animations/BP_PioneerAnimation.BP_PioneerAnimation_C'";
	UClass* animBP = LoadObject<UClass>(NULL, *animBP_Reference);
	if (!animBP)
	{
		UE_LOG(LogTemp, Warning, TEXT("!animBP"));
	}
	else
		GetMesh()->SetAnimInstanceClass(animBP);

	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;

	//// 5.1 AnimInstance�� ������� �ʰ� �����ϰ� �ִϸ��̼��� ����Ϸ��� AnimSequence�� �����ͼ� Skeleton�� �����մϴ�.
	//static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Mannequin/Animations/ThirdPersonRun.ThirdPersonRun'"));
	//if (animSequence.Succeeded())
	//{
	//	AnimSequence = animSequence.Object;
	//	AnimSequence->SetSkeleton(Skeleton);
	//	GetMesh()->PlayAnimation(AnimSequence, true); // SkeletalMeshComp�� AnimSequence�� �ش��ϴ� �ִϸ��̼��� ����մϴ�. 2��° ���ڴ� �������� �Դϴ�.
	//}
	//// 5.2 ��Ÿ�ָ� ����Ͽ� �� ������ �ִϸ��̼��� ����� �� �ֽ��ϴ�. UAnimMontage* AnimMontage;
	//{
	//	PlayAnimMontage();
	//	GetMesh()->GetAnimInstance()->Montage_Play();
	//	// takes in a blend speed ( 1.0f ) as well as the montage
	//	AnimInstance->Montage_Stop(1.0f, AttackMontage->Montage);
	//	// takes in the montage you want to pause
	//	AnimInstance->Montage_Pause(AttackMontage->Montage);
	//	// takes in the montage you want to resume playback for
	//	AnimInstance->Montage_Resume(AttackMontage->Montage);
	//	// kicks off the playback at a steady rate of 1 and starts playback at 0 frames
	//	AnimInstance->Montage_Play(AttackMontage->Montage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
	//	// load player attack montage data table
	//	static ConstructorHelpers::FObjectFinder<UDataTable> PlayerAttackMontageDataObject(TEXT("DataTable'/Game/DataTables/PlayerAttackMontageDataTable.PlayerAttackMontageDataTable'"));
	//	if (PlayerAttackMontageDataObject.Succeeded())
	//	{
	//		PlayerAttackDataTable = PlayerAttackMontageDataObject.Object;
	//	}
	//	// set animation blending on by default
	//	bIsAnimationBlended = true;
	//}
}

bool APioneer::HasPistolType()
{
	return bHasPistolType;
}
bool APioneer::HasRifleType()
{
	return bHasRifleType;
}
bool APioneer::HasLauncherType()
{
	return bHasLauncherType;
}

void APioneer::SetIsKeyboardEnabled(bool Enabled)
{
	bIsKeyboardEnabled = Enabled;
}
bool APioneer::IsAnimationBlended()
{
	return bIsAnimationBlended;
}
/*** SkeletalAnimation : End ***/

/*** Camera : Start ***/
void APioneer::InitCamera()
{
	/*** ī�޶� ������ PIE�� �����մϴ�. : Start ***/
	CameraBoomLocation = FVector(-300.0f, 0.0f, 300.0f); // ArmSpring�� World ��ǥ�Դϴ�.
	CameraBoomRotation = FRotator(-60.f, 0.f, 0.f); // ArmSpring�� World ȸ���Դϴ�.
	TargetArmLength = 1500.0f; // ArmSpring�� CameraComponent���� �Ÿ��Դϴ�.
	CameraLagSpeed = 3.0f; // �ε巯�� ī�޶� ��ȯ �ӵ��Դϴ�.
	/*** ī�޶� ������ PIE�� �����մϴ�. : End ***/

	// Cameraboom�� �����մϴ�. (�浹 �� �÷��̾� ������ �ٰ��� ��ġ�մϴ�.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // ĳ���Ͱ� ȸ���� �� Arm�� ȸ����Ű�� �ʽ��ϴ�. ���� ��ǥ���� ȸ���� �������� �մϴ�.
	CameraBoom->TargetArmLength = 1500.0f; // �ش� �������� ī�޶� Arm�� ����ٴմϴ�.
	CameraBoom->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraBoom->SetRelativeLocation(FVector(-300.0f, 0.0f, 300.0f));

	//CameraBoom->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����Ű�� �ʽ��ϴ�.
	CameraBoom->bDoCollisionTest = false; // Arm�� ī�޶� ������ ������ � ��ü�� �浹���� �� ���� �ʵ��� ī�޶� ����� �ʽ��ϴ�.
	CameraBoom->bEnableCameraLag = true; // �̵��� �ε巯�� ī�޶� ��ȯ�� ���� �����մϴ�.
	CameraBoom->CameraLagSpeed = 3.0f; // ī�޶� �̵��ӵ��Դϴ�.

	// ����ٴϴ� ī�޶� �����մϴ�.
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // boom�� �� ���ʿ� �ش� ī�޶� ���̰�, ��Ʈ�ѷ��� ���⿡ �°� boom�� �����մϴ�.
	TopDownCameraComponent->bUsePawnControlRotation = false;
}

void APioneer::SetCameraBoomSettings()
{
	// ��ô�� ���� ��ġ�� ã���ϴ�.
	FVector rootComponentLocation = RootComponent->GetComponentLocation();

	// Pioneer�� ���� ��ġ���� position ��ŭ ���ϰ� rotation�� �����մϴ�.
	CameraBoom->SetWorldLocationAndRotation(
		FVector(rootComponentLocation.X + CameraBoomLocation.X, rootComponentLocation.Y + CameraBoomLocation.Y, rootComponentLocation.Z + CameraBoomLocation.Z),
		CameraBoomRotation);

	CameraBoom->TargetArmLength = TargetArmLength; // ĳ���� �ڿ��� �ش� �������� ����ٴϴ� ī�޶�
	CameraBoom->CameraLagSpeed = CameraLagSpeed;
}

void APioneer::ZoomInOrZoomOut(float Value)
{
	TargetArmLength += Value * 64.0f;

	if (TargetArmLength < 0.0f)
		TargetArmLength = 0.0f;
	/*else if (TargetArmLength > 1500.0f)
		TargetArmLength = 1500.0f;*/
}
/*** Camera : End ***/

/*** Cursor : Start ***/
void APioneer::InitCursor()
{
	// Create a decal in the world to show the cursor's location
	// A material that is rendered onto the surface of a mesh. A kind of 'bumper sticker' for a model.
	// ������� Ŀ���� ��ġ�� ǥ���� ��Į�� �����մϴ�.
	// ��Į�� �޽��� ǥ�鿡 �������� ���͸����Դϴ�.
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/BluePrints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
}

void APioneer::SetCursorToWorld()
{
	if (!CursorToWorld || !GetController())
		return;

	if (GetController() == AIController)
	{
		CursorToWorld->SetVisibility(false);
		return;
	}


	/*if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (UWorld* World = GetWorld())
		{
			FHitResult HitResult;
			FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
			FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
			FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
			Params.AddIgnoredActor(this);
			World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
			FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
			CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
		}
	}*/
	//// �� �ڵ�� Collisionä���� ECC_Visibility�� ���� ����� ���� ������ ������.
	//else if (APlayerController* PC = Cast<APlayerController>(GetController()))
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult TraceHitResult;
		PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
		FVector CursorFV = TraceHitResult.ImpactNormal;
		FRotator CursorR = CursorFV.Rotation();
		CursorToWorld->SetWorldLocation(TraceHitResult.Location);
		CursorToWorld->SetWorldRotation(CursorR);

		// ���Ⱑ �ִٸ� Ŀ�� ��ġ�� �ٶ󺾴ϴ�. ������ �ٶ��� �ʽ��ϴ�.
		if (CurrentWeapon)
		{ 
			LookAtTheLocation(CursorToWorld->GetComponentLocation());
		}

		CursorToWorld->SetVisibility(true);
	}

	//// �� �ڵ�� LineTrace�� �� ��� ���͸� hit�ϰ� �� �� LandScape�� �����ͼ� ���콺 Ŀ�� Transform ������ ����.
	//if (UWorld* World = GetWorld())
	//{
	//	// ���� Player�� ����Ʈ�� ���콺�������� �����ɴϴ�.
	//	APlayerController* PC = Cast<APlayerController>(GetController());
	//	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
	//	FVector2D MousePosition;
	//	if (LocalPlayer && LocalPlayer->ViewportClient)
	//	{
	//		LocalPlayer->ViewportClient->GetMousePosition(MousePosition);
	//	}

	//	FVector WorldOrigin; // ���� ��ġ
	//	FVector WorldDirection; // ����
	//	float HitResultTraceDistance = 100000.f; // WorldDirection�� ���Ͽ� �� ��ġ�� ����
	//	UGameplayStatics::DeprojectScreenToWorld(PC, MousePosition, WorldOrigin, WorldDirection);
	//	FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // ��� ������Ʈ
	//	//FCollisionQueryParams& CollisionQueryParams()

	//	TArray<FHitResult> hitResults; // ����� ����
	//	World->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjectQueryParams);

	//	for (auto& hit : hitResults)
	//	{
	//		//if (hit.Actor->GetClass() == ALandscape::StaticClass())
	//		//if (Cast<ALandscape>(hit.Actor))
	//		if (hit.Actor->IsA(ALandscape::StaticClass())) // hit�� Actor�� ALandscape��
	//		{
	//			FVector CursorFV = hit.ImpactNormal;
	//			FRotator CursorR = CursorFV.Rotation();
	//			CursorToWorld->SetWorldLocation(hit.Location);
	//			CursorToWorld->SetWorldRotation(CursorR);

	//			// ���Ⱑ �ִٸ� Ŀ�� ��ġ�� �ٶ󺾴ϴ�. ������ �ٶ��� �ʽ��ϴ�.
	//			if (Weapon)
	//			{ 
	//				LookAtTheLocation(CursorToWorld->GetComponentLocation());
	//			}

	//			CursorToWorld->SetVisibility(true);
	//		}
	//	}
	//}

}
/*** Cursor : End ***/

/*** APioneerAIController : Start ***/
void APioneer::InitAIController()
{
	Super::InitAIController();

	// �̹� AIController�� ������ ������ �������� ����.
	if (AIController)
		return;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::InitAIController(): !World"));
		return;
	}

	FTransform myTrans = GetTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AIController = World->SpawnActor<APioneerAIController>(APioneerAIController::StaticClass(), myTrans, SpawnParams);

	// Controller�� Attach�� �ȵ�.
}

void APioneer::PossessAIController()
{
	ABaseCharacter::PossessAIController();


}
/*** APioneerAIController : End ***/

/*** Weapon : Start ***/
void APioneer::InitWeapon()
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::InitWeapon: !World"));
		return;
	}

	FTransform myTrans = FTransform::Identity;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	// ��ô�ڴ� �⺻������ ������ ������ ����
	APistol* Pistol = World->SpawnActor<APistol>(APistol::StaticClass(), myTrans, SpawnParams);
	Pistol->Acquired();
	Pistol->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	//Pistol->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(Pistol) == false)
	{
		IdxOfCurrentWeapon = Weapons.Add(Pistol);
		Arming();
	}

	/*** �ӽ� �ڵ� : Start ***/
	AAssaultRifle* assaultRifle = World->SpawnActor<AAssaultRifle>(AAssaultRifle::StaticClass(), myTrans, SpawnParams);
	assaultRifle->Acquired();
	assaultRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("AssaultRifleSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	assaultRifle->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(assaultRifle) == false)
		Weapons.Add(assaultRifle);

	AGrenadeLauncher* grenadeLauncher = World->SpawnActor<AGrenadeLauncher>(AGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
	grenadeLauncher->Acquired();
	grenadeLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GrenadeLauncherSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	grenadeLauncher->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(grenadeLauncher) == false)
		Weapons.Add(grenadeLauncher);

	Pistol = World->SpawnActor<APistol>(APistol::StaticClass(), myTrans, SpawnParams);
	Pistol->Acquired();
	Pistol->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	Pistol->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(Pistol) == false)
		Weapons.Add(Pistol);

	ARocketLauncher* rocketLauncher = World->SpawnActor<ARocketLauncher>(ARocketLauncher::StaticClass(), myTrans, SpawnParams);
	rocketLauncher->Acquired();
	rocketLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RocketLauncherSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	rocketLauncher->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(rocketLauncher) == false)
		Weapons.Add(rocketLauncher);

	AShotgun* shotgun = World->SpawnActor<AShotgun>(AShotgun::StaticClass(), myTrans, SpawnParams);
	shotgun->Acquired();
	shotgun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ShotgunSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	shotgun->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(shotgun) == false)
		Weapons.Add(shotgun);

	ASniperRifle* sniperRifle = World->SpawnActor<ASniperRifle>(ASniperRifle::StaticClass(), myTrans, SpawnParams);
	sniperRifle->Acquired();
	sniperRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SniperRifleSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	sniperRifle->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(sniperRifle) == false)
		Weapons.Add(sniperRifle);
	/*** �ӽ� �ڵ� : End ***/
}

void APioneer::AcquireWeapon(class AWeapon* weapon)
{
	if (!weapon || !GetMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::AcquireWeapon: if (!weapon || !GetMesh())"));
		return;
	}

	// ���� ��������
	Disarming();

	OverlapedItems.RemoveSingle(weapon);

	weapon->Acquired();
	weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), weapon->SocketName);

	CurrentWeapon = weapon;
	IdxOfCurrentWeapon = Weapons.Add(CurrentWeapon);

	// �ٽ� ȹ���� ����� ����
	Arming();
}
void APioneer::AbandonWeapon()
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::AbandonWeapon: if (!CurrentWeapon)"));
		return;
	}

	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentWeapon->Droped();

	Weapons.RemoveSingle(CurrentWeapon);
	CurrentWeapon = nullptr;

	SetWeaponType();

	GetCharacterMovement()->bOrientRotationToMovement = true; // ���⸦ ���� ������ �̵� ���⿡ ĳ���� �޽ð� ���� ȸ���մϴ�.

	Arming();
}

void APioneer::FireWeapon()
{
	if (CurrentWeapon)
	{
		// ��Ÿ���� ���ƿͼ� �߻簡 �Ǿ��ٸ� UPioneerAnimInstance�� �˷��ݴϴ�.
		if (CurrentWeapon->Fire())
		{
			// Pistol�� Fire �ִϸ��̼��� ��� �����մϴ�.
			if (CurrentWeapon->IsA(APistol::StaticClass()) == false)
			{
				// ������� BP_PioneerAnimation�� �����ͼ� bFired ������ �����մϴ�.
				UPioneerAnimInstance* PAnimInst = dynamic_cast<UPioneerAnimInstance*>(GetMesh()->GetAnimInstance());
				if (PAnimInst)
					PAnimInst->bFired = true;
			}

			State = EPioneerFSM::Idle;
		}
	}
}

void APioneer::SetWeaponType()
{
	// �⺻������ BP_PioneerAnimation���� ���� ��������
	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;

	// ���� ���⸦ �� ���¿��� ���� ���� ����
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::SetWeaponType: if (!CurrentWeapon)"));
		return;
	}

	// �´� ���⸦ ��Բ� �ϱ�
	switch (CurrentWeapon->WeaponType)
	{
	case EWeaponType::Pistol:
		bHasPistolType = true;
		break;
	case EWeaponType::Rifle:
		bHasRifleType = true;
		break;
	case EWeaponType::Launcher:
		bHasLauncherType = true;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("APioneer::SetWeaponType: switch (CurrentWeapon->WeaponType) default:"));
		break;
	}
}

void APioneer::ChangeWeapon(int Value)
{
	/*for (auto It = StrArr.CreateConstIterator(); It; ++It)
	{
		JoinedStr += *It;
		JoinedStr += TEXT(" ");
	}*/

	// ���� ���⸦ �� ���¿��� ���� ���� ����
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::ChangeWeapon: if (!CurrentWeapon)"));
		return;
	}

	// Weapon�� 2�� �̻� �־�� ���� ���� ����
	if (Weapons.Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::ChangeWeapon: if (Weapons.Num() == 0)"));
		return;
	}

	CurrentWeapon->SetActorHiddenInGame(true);

	int32 start = IdxOfCurrentWeapon;
	int32 end = (Value == 1) ? Weapons.Num() : 0;

	for (int32 idx{ start }; 
		Weapons.IsValidIndex(idx); // �ε����� ��ȿ���� �ʴٸ� �ǳʶ��
		idx += Value)
	{
		// ������ ���Ⱑ ���ų� ���� ������ �ǳʶ��
		if (Weapons[idx] == nullptr || Weapons[idx] == CurrentWeapon)
			continue;

		IdxOfCurrentWeapon = idx;
		break;
	}

	CurrentWeapon = Weapons[IdxOfCurrentWeapon];
	CurrentWeapon->SetActorHiddenInGame(false);
	SetWeaponType();
}
void APioneer::Arming()
{
	// IdxOfCurrentWeapon�� ��ȿ���� ������
	if (Weapons.IsValidIndex(IdxOfCurrentWeapon) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::Arming: if (IdxOfCurrentWeapon == INDEX_NONE)"));

		CurrentWeapon = nullptr;

		// Weapons�� �����ϴ� ���� ���� Weapon�� ����
		for (auto& weapon : Weapons)
		{
			if (weapon)
			{
				CurrentWeapon = weapon;
				IdxOfCurrentWeapon = Weapons.IndexOfByKey(CurrentWeapon);
				break;
			}
		}
	}
	else // IdxOfCurrentWeapon�� ��ȿ�ϸ�
	{
		if (Weapons[IdxOfCurrentWeapon])
			CurrentWeapon = Weapons[IdxOfCurrentWeapon];
		else
		{
			// Weapons�� �����ϴ� ���� ���� Weapon�� ����
			for (auto& weapon : Weapons)
			{
				if (weapon)
				{
					CurrentWeapon = weapon;
					IdxOfCurrentWeapon = Weapons.IndexOfByKey(CurrentWeapon);
					break;
				}
			}
		}
	}

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::Arming: if (!CurrentWeapon)"));
		return;
	}

	CurrentWeapon->SetActorHiddenInGame(false);

	SetWeaponType();

	if (GetCharacterMovement())
		GetCharacterMovement()->bOrientRotationToMovement = false; // ���⸦ ��� �̵� ���⿡ ĳ���� �޽ð� ���� ȸ������ �ʽ��ϴ�.
}
void APioneer::Disarming()
{
	if (CurrentWeapon)
		CurrentWeapon->SetActorHiddenInGame(true);

	CurrentWeapon = nullptr;

	SetWeaponType();

	if (GetCharacterMovement())
		GetCharacterMovement()->bOrientRotationToMovement = true; // ���⸦ ���� ������ �̵� ���⿡ ĳ���� �޽ð� ���� ȸ���մϴ�.
}
/*** Weapon : End ***/

/*** Building : Start ***/
void APioneer::SpawnBuilding(int Value)
{
	DestroyBuilding();

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::SpawnBuilding: if (!World)"));
		return;
	}

	FTransform myTrans = FTransform::Identity;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	switch ((EBuildingType)Value)
	{
	case EBuildingType::Floor:
		Building = World->SpawnActor<AFloor>(AFloor::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Wall:
		Building = World->SpawnActor<AWall>(AWall::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Stairs:
		Building = World->SpawnActor<AStairs>(AStairs::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Turret:
		Building = World->SpawnActor<ATurret>(ATurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Gate:
		Building = World->SpawnActor<AGate>(AGate::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::OrganicMine:
		Building = World->SpawnActor<AOrganicMine>(AOrganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::InorganicMine:
		Building = World->SpawnActor<AInorganicMine>(AInorganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::NuclearFusionPowerPlant:
		Building = World->SpawnActor<ANuclearFusionPowerPlant>(ANuclearFusionPowerPlant::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::ResearchInstitute:
		Building = World->SpawnActor<AResearchInstitute>(AResearchInstitute::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::WeaponFactory:
		Building = World->SpawnActor<AWeaponFactory>(AWeaponFactory::StaticClass(), myTrans, SpawnParams);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("APioneer::SpawnBuilding: if (!World)"));
		break;
	}
}

void APioneer::OnConstructingMode()
{
	if (!bConstructingMode || !CursorToWorld || !Building)
	{
		//UE_LOG(LogTemp, Warning, TEXT("APioneer::OnConstructingMode: if (!bConstructingMode || !CursorToWorld || !Building)"));
		return;
	}

	if (GetController() == AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::OnConstructingMode: if (GetController() == AIController)"));
		return;
	}

	// �� �ڵ�� LineTrace�� �� ��� ���͸� hit�ϰ� �� �� LandScape�� �����ͼ� ���콺 Ŀ�� Transform ������ ����.
	if (UWorld* World = GetWorld())
	{
		// ���� Player�� ����Ʈ�� ���콺�������� �����ɴϴ�.
		APlayerController* PC = Cast<APlayerController>(GetController());
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
		FVector2D MousePosition;
		if (LocalPlayer && LocalPlayer->ViewportClient)
		{
			LocalPlayer->ViewportClient->GetMousePosition(MousePosition);
		}

		FVector WorldOrigin; // ���� ��ġ
		FVector WorldDirection; // ����
		float HitResultTraceDistance = 100000.f; // WorldDirection�� ���Ͽ� �� ��ġ�� ����
		UGameplayStatics::DeprojectScreenToWorld(PC, MousePosition, WorldOrigin, WorldDirection);
		FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // ��� ������Ʈ
		//FCollisionQueryParams& CollisionQueryParams()

		TArray<FHitResult> hitResults; // ����� ����
		World->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjectQueryParams);

		for (auto& hit : hitResults)
		{
			//if (hit.Actor->GetClass() == ALandscape::StaticClass())
			//if (Cast<ALandscape>(hit.Actor))
			if (hit.Actor->IsA(ALandscape::StaticClass())) // hit�� Actor�� ALandscape��
			{
				Building->SetActorLocation(hit.Location);
			}
		}
	}
}

void APioneer::RotatingBuilding(float Value)
{
	if (!Building || !bConstructingMode)
		return;

	Building->Rotating(Value);
}
void APioneer::PlaceBuilding()
{
	if (!bConstructingMode || !Building)
		return;

	bool success = Building->Constructing();
	if (success)
	{
		Building = nullptr;
		bConstructingMode = false;
	}
}
void APioneer::DestroyBuilding()
{
	if (Building)
	{
		Building->Destroying();
		Building = nullptr;
	}
}
/*** Building : End ***/

/*** Equipments : Start ***/
void APioneer::InitEquipments()
{
	HelmetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelmetMesh"));

	// (��Ű¡ ���� ����: �ٸ� ���͸� ���� �� AttachToComponent�� ��������� ���۳�Ʈ�� ���� �� SetupAttachment�� ����ؾ� �Ѵ�.)
	//HelmetMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("HeadSocket"));
	HelmetMesh->SetupAttachment(GetMesh(), TEXT("HeadSocket"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> helmetMesh(TEXT("StaticMesh'/Game/Characters/Equipments/LowHelmet/Lowhelmet.Lowhelmet'"));
	if (helmetMesh.Succeeded())
	{
		HelmetMesh->SetStaticMesh(helmetMesh.Object);
	}
}
/*** Equipments : End ***/

/*** FSM : Start ***/
void APioneer::InitFSM()
{
	State = EPioneerFSM::Idle;
}

void APioneer::RunFSM(float DeltaTime)
{
	switch (State)
	{
	case EPioneerFSM::Idle:
	{
		IdlingOfFSM();
		break;
	}
	case EPioneerFSM::Tracing:
	{
		TracingOfFSM();
		break;
	}
	case EPioneerFSM::Attack:
	{
		// ���� �ִϸ��̼��� ���� �� AnimationBlueprint���� EventGraph�� Atteck�� Idle�� �ٲ���
		AttackingOfFSM();
		break;
	}
	}
}

void APioneer::FindTheTargetActor()
{
	TargetActor = nullptr;

	for (auto& actor : OverapedDetectRangeActors)
	{
		if (actor->IsA(AEnemy::StaticClass()))
		{
			// AEnemy�� �׾��ִٸ� skip
			if (AEnemy* enemy = Cast<AEnemy>(actor))
			{
				if (enemy->bDying)
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
}

void APioneer::IdlingOfFSM()
{
	FindTheTargetActor();

	if (TargetActor)
		State = EPioneerFSM::Tracing;
}
void APioneer::TracingOfFSM()
{
	FindTheTargetActor();

	TracingTargetActor();

	if (!TargetActor)
	{
		State = EPioneerFSM::Idle;
		GetController()->StopMovement();
		return;
	}
	else if (OverapedAttackRangeActors.Num() > 0)
	{
		State = EPioneerFSM::Attack;
		GetController()->StopMovement();
		return;
	}
}
void APioneer::AttackingOfFSM()
{
	FireWeapon();
}
/*** FSM : End ***/

/*** BehaviorTree : Start ***/
void APioneer::RunBehaviorTree(float DeltaTime)
{
	Super::RunBehaviorTree(DeltaTime);

}
/*** BehaviorTree : End ***/


/*** Item : Start ***/
void APioneer::InitItem()
{
	if (!GetCapsuleComponent())
		return;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APioneer::OnOverlapBegin_Item);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APioneer::OnOverlapEnd_Item);
}
void APioneer::OnOverlapBegin_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(AItem::StaticClass()))
	{
		if (AItem* item = Cast<AItem>(OtherActor))
		{
			if (OtherComp == item->GetInteractionRange())
				OverlapedItems.Add(item);
		}
	}
}
void APioneer::OnOverlapEnd_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(AItem::StaticClass()))
	{
		if (AItem* item = Cast<AItem>(OtherActor))
		{
			if (OtherComp == item->GetInteractionRange())
				OverlapedItems.RemoveSingle(item);
		}
	}
}
/*** Item : Start ***/












//void APioneer::PunchAttack()
//{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("PunchAttack"));
//	AttackInput(EAttackType::MELEE_FIST);
//}
//
//void APioneer::KickAttack()
//{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("KickAttack"));
//	AttackInput(EAttackType::MELEE_KICK);
//}
//
//void APioneer::AttackInput(EAttackType AttackType)
//{
//	if (PlayerAttackDataTable)
//	{
//		static const FString ContextString(TEXT("Player Attack Montage Context"));
//
//		FName RowKey;
//
//		// attach collision components to sockets based on transformations definitions
//		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
//
//		switch (AttackType)
//		{
//		case EAttackType::MELEE_FIST:
//			RowKey = FName(TEXT("Punch"));
//
//			//LeftMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "fist_l_collision");
//			//RightMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "fist_r_collision");
//
//			bIsKeyboardEnabled = true;
//
//			bIsAnimationBlended = true;
//			break;
//		case EAttackType::MELEE_KICK:
//			RowKey = FName(TEXT("Kick"));
//
//			//LeftMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "foot_l_collision");
//			//RightMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "foot_r_collision");
//
//			bIsKeyboardEnabled = false;
//
//			bIsAnimationBlended = false;
//			break;
//		default:
//
//			bIsAnimationBlended = true;
//			break;
//		}
//
//		AttackMontage = PlayerAttackDataTable->FindRow<FPlayerAttackMontage>(RowKey, ContextString, true);
//
//		if (AttackMontage)
//		{
//			// pick the correct montage section based on our attack type
//			int MontageSectionIndex;
//			MontageSectionIndex = 1;
//
//			// create a montage section
//			FString MontageSection = "start_" + FString::FromInt(MontageSectionIndex);
//
//			PlayAnimMontage(AttackMontage->Montage, 1.f, FName(*MontageSection));
//
//			if (!bIsArmed)
//			{
//				bIsArmed = true;
//			}
//		}
//	}
//	else
//		UE_LOG(LogTemp, Warning, TEXT("Failed: PlayerAttackDataTable"));
//}

//void APioneer::AttackStart()
//{
//	//Log(ELogLevel::INFO, __FUNCTION__);
//
//	/*LeftMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
//	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(true);
//
//	RightMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
//	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(true);*/
//}
//
//void APioneer::AttackEnd()
//{
//	//Log(ELogLevel::INFO, __FUNCTION__);
//
//	/*LeftMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
//	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(false);
//
//	RightMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
//	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(false);*/
//
//	UWorld* const world = GetWorld();
//	if (!world)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
//		return;
//	}
//
//	bool bIsActive = world->GetTimerManager().IsTimerActive(ArmedToIdleTimerHandle);
//	if (bIsActive)
//	{
//		// reset the timer
//		world->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
//	}
//
//	CountdownToIdle = MaxCountdownToIdle;
//
//	// start timer from scratch
//	world->GetTimerManager().SetTimer(ArmedToIdleTimerHandle, this, &APioneer::TriggerCountdownToIdle, 1.f, true);
//}
//void APioneer::TriggerCountdownToIdle()
//{
//	// count down to zero
//	if (--CountdownToIdle <= 0) {
//		bIsArmed = false;
//		CountdownToIdle = MaxCountdownToIdle;
//
//		UWorld* const world = GetWorld();
//		if (!world)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
//			return;
//		}
//		world->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
//	}
//}