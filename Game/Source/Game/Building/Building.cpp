// Fill out your copyright notice in the Description page of Project Settings.

#include "Building.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/Projectile.h"
#include "Character/BaseCharacter.h"

#include "Network/Packet.h"
#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "PioneerManager.h"

#include "BuildingManager.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ABuilding::ABuilding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->SetGenerateOverlapEvents(false);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SphereComponent;


	InitStat();


	InitHelthPointBar();


	InitConstructBuilding();


	InitBuilding();


	InitMaterial();


	BuildingState = EBuildingState::Constructable;

	TimerOfTickOfConsumeAndProduct = 0.0f;

	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;

	ID = 0;

	BuildingManager = nullptr;
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();


	// Pioneer�� �����Ѱ� �ƴ϶�� �ǹ��� �ٷ� �ϼ���Ŵ
	if (!GetOwner())
	{
		// ����Ŭ���̾�Ʈ��� ���Ӽ������� SpawnBuilding���� �����ϱ� ������ �Ҹ��ŵ�ϴ�.
		if (ClientSocketInGame)
		{
			if (ClientSocketInGame->IsClientSocketOn())
			{
				Destroy();
				return;
			}
		}

		BuildingState = EBuildingState::Constructed;
		CompleteConstructing();
	}

	BeginPlayHelthPointBar();

	
}

void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BuildingState == EBuildingState::Constructable)
	{
		if (OverapedActors.Num() > 0)
			SetUnConstructableMaterial();
		else
			SetConstructableMaterial();
	}

	TickHelthPointBar();

	TickOfConsumeAndProduct(DeltaTime);


	// �ӽ�
	SetHealthPoint(NULL);
}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ABuilding::InitHelthPointBar()
{
	HelthPointBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HelthPointBar"));
	//HelthPointBar = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass());
	HelthPointBar->SetupAttachment(RootComponent);
	HelthPointBar->bAbsoluteRotation = true; // �������� ȸ������ �����մϴ�.

	HelthPointBar->SetOnlyOwnerSee(false);
	//HelthPointBar->SetIsReplicated(false);

	HelthPointBar->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	//HelthPointBar->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	HelthPointBar->SetRelativeRotation(FRotator(45.0f, 180.0f, 0.0f)); // �׻� �÷��̾�� ���̵��� ȸ�� ���� World�� �ؾ� ��.
	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 30));

	// Screen�� ����Ʈ���� UIó�� ����ִ� ���̰� World�� ���� ������ UIó�� ����ִ� ��
	HelthPointBar->SetWidgetSpace(EWidgetSpace::World);
}
void ABuilding::BeginPlayHelthPointBar()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::BeginPlayHelthPointBar() Failed: UWorld* const World = GetWorld();"));
		return;
	}

	/*** ����: Blueprint �ּ��� �ڿ� _C�� �ٿ��༭ Ŭ������ ��������� ��. ***/
	FString HelthPointBarBP_Reference = "WidgetBlueprint'/Game/Characters/HelthPointBar.HelthPointBar_C'";
	UClass* HelthPointBarBP = LoadObject<UClass>(this, *HelthPointBarBP_Reference);

	// ������ WidgetBlueprint�� UWidgetComponent�� �ٷ� ������������ ���� UUserWidget�� �����Ͽ� ������ �� ��
	// UWidgetComponent->SetWidget(������ UUserWidget);���� UWidgetComponent�� �����ؾ� ��.
	//HelthPointBar->SetWidgetClass(HelthPointBarBP);
	HelthPointBarUserWidget = CreateWidget(world, HelthPointBarBP); // wolrd�� �� �ʿ�.

	if (HelthPointBarUserWidget)
	{
		UWidgetTree* WidgetTree = HelthPointBarUserWidget->WidgetTree;
		if (WidgetTree)
		{
			//// �� ����� �ȵ�.
			// ProgreeBar = Cast<UProgressBar>(HelthPointBarUserWidget->GetWidgetFromName(FName(TEXT("ProgressBar_153"))));

			ProgressBar = WidgetTree->FindWidget<UProgressBar>(FName(TEXT("ProgressBar_153")));
			if (ProgressBar == nullptr)
				UE_LOG(LogTemp, Warning, TEXT("ProgressBar == nullptr"));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("WidgetTree == nullptr"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("HelthPointBarUserWidget == nullptr"));

	HelthPointBar->SetWidget(HelthPointBarUserWidget);
}

void ABuilding::TickHelthPointBar()
{
	if (ProgressBar)
		ProgressBar->SetPercent(HealthPoint / MaxHealthPoint);
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void ABuilding::InitStat()
{
	/// Default Settings
	ConstructionTime = 2.0f;

	HealthPoint = 10.0f;
	MaxHealthPoint = 100.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 0.0f;
	NeedOrganicMatter = 0.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 0.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}

void ABuilding::InitConstructBuilding()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void ABuilding::InitBuilding()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void ABuilding::InitMaterial()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> constructableMaterial(TEXT("Material'/Game/Buildings/Constructable.Constructable'"));
	if (constructableMaterial.Succeeded())
	{
		ConstructableMaterial = constructableMaterial.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> unConstructableMaterial(TEXT("Material'/Game/Buildings/UnConstructable.UnConstructable'"));
	if (unConstructableMaterial.Succeeded())
	{
		UnConstructableMaterial = unConstructableMaterial.Object;
	}
}


void ABuilding::AddConstructBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale, FRotator Rotation, FVector Location)
{
	(*StaticMeshComp) = CreateDefaultSubobject<UStaticMeshComponent>(CompName);
	(*StaticMeshComp)->SetupAttachment(RootComponent);

	(*StaticMeshComp)->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin_ConstructBuilding);
	(*StaticMeshComp)->SetGenerateOverlapEvents(false);
	(*StaticMeshComp)->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	(*StaticMeshComp)->SetHiddenInGame(true);

	// static Ű���带 �����Ͽ� �ν��Ͻ����� �ּ�(���ҽ�)�� �� �� ���� �ε��մϴ�. (����: static�� ������ �� ���� ���� �˴ϴ�.)
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(ObjectToFind);
	if (staticMesh.Succeeded())
	{
		(*StaticMeshComp)->SetStaticMesh(staticMesh.Object);

		// StaticMesh�� ���� ������ ����
		FVector minBounds, maxBounds;
		(*StaticMeshComp)->GetLocalBounds(minBounds, maxBounds);
		//UE_LOG(LogTemp, Warning, TEXT("%s minBounds: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), minBounds.X, minBounds.Y, minBounds.Z);
		//UE_LOG(LogTemp, Warning, TEXT("%s maxBounds: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), maxBounds.X, maxBounds.Y, maxBounds.Z);


		// RootComponent�� SphereComponent�� StaticMesh�� �ϴ� ���߾����� ���Բ� ��������� ��.
		// ������ S->R->T ������ �ؾ� �������� ����� ����.
		(*StaticMeshComp)->SetRelativeScale3D(Scale);
		(*StaticMeshComp)->SetRelativeRotation(Rotation);
		FVector center;
		center.X = -1.0f * ((maxBounds.X * Scale.X + minBounds.X * Scale.X) / 2.0f);
		center.Y = -1.0f * ((maxBounds.Y * Scale.Y + minBounds.Y * Scale.Y) / 2.0f);
		center.Z = -1.0f * (minBounds.Z * Scale.Z);
		(*StaticMeshComp)->SetRelativeLocation(center + Location);

		//UE_LOG(LogTemp, Warning, TEXT("%s center: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), center.X, center.Y, center.Z);
	}

	ConstructBuildingSMCs.Add(*StaticMeshComp);
}

void ABuilding::AddBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale, FRotator Rotation, FVector Location)
{
	(*StaticMeshComp) = CreateDefaultSubobject<UStaticMeshComponent>(CompName);
	(*StaticMeshComp)->SetupAttachment(RootComponent);
	(*StaticMeshComp)->SetVisibility(true);

	(*StaticMeshComp)->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin_Building);
	(*StaticMeshComp)->OnComponentEndOverlap.AddDynamic(this, &ABuilding::OnOverlapEnd_Building);

	(*StaticMeshComp)->SetGenerateOverlapEvents(true);
	(*StaticMeshComp)->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	(*StaticMeshComp)->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	(*StaticMeshComp)->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	// (�⺻������ true�� �Ǿ� ����) NavMesh�� ������Ʈ �ǵ��� CanEverAffectNavigation�� true�� ����.
	//StaticMeshComp->SetCanEverAffectNavigation(true);

	// static Ű���带 �����Ͽ� �ν��Ͻ����� �ּ�(���ҽ�)�� �� �� ���� �ε��մϴ�. (����: static�� ������ �� ���� ���� �˴ϴ�.)
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshComp(ObjectToFind);
	if (staticMeshComp.Succeeded())
	{
		(*StaticMeshComp)->SetStaticMesh(staticMeshComp.Object);

		// StaticMesh�� ���� ������ ����
		FVector minBounds, maxBounds;
		(*StaticMeshComp)->GetLocalBounds(minBounds, maxBounds);
		//UE_LOG(LogTemp, Warning, TEXT("%s minBounds: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), minBounds.X, minBounds.Y, minBounds.Z);
		//UE_LOG(LogTemp, Warning, TEXT("%s maxBounds: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), maxBounds.X, maxBounds.Y, maxBounds.Z);

		// RootComponent�� SphereComponent�� StaticMesh�� �ϴ� ���߾����� ���Բ� ��������� ��.
		// ������ S->R->T ������ �ؾ� �������� ����� ����.
		(*StaticMeshComp)->SetRelativeScale3D(Scale);
		(*StaticMeshComp)->SetRelativeRotation(Rotation);
		FVector center;
		center.X = -1.0f * ((maxBounds.X * Scale.X + minBounds.X * Scale.X) / 2.0f);
		center.Y = -1.0f * ((maxBounds.Y * Scale.Y + minBounds.Y * Scale.Y) / 2.0f);
		center.Z = -1.0f * (minBounds.Z * Scale.Z);
		(*StaticMeshComp)->SetRelativeLocation(center + Location);

		// ���� ���͸��� ����
		FTArrayOfUMaterialInterface TArrayOfUMaterialInterface;
		TArrayOfUMaterialInterface.Object = (*StaticMeshComp)->GetMaterials();
		BuildingSMCsMaterials.Add(TArrayOfUMaterialInterface);

		//UE_LOG(LogTemp, Warning, TEXT("%s center: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), center.X, center.Y, center.Z);
	}

	BuildingSMCs.Add(*StaticMeshComp);

}

void ABuilding::AddBuildingSkMC(USkeletalMeshComponent** SkeletalMeshComp, UStaticMeshComponent** SubStaticMeshComp,
	const TCHAR* CompName, const TCHAR* ObjectToFind,
	FVector Scale, FRotator Rotation, FVector Location)
{
	(*SkeletalMeshComp) = CreateDefaultSubobject<USkeletalMeshComponent>(CompName);
	(*SkeletalMeshComp)->SetupAttachment(RootComponent);
	(*SkeletalMeshComp)->SetVisibility(true);

	/*(*SkeletalMeshComp)->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin_Building);
	(*SkeletalMeshComp)->OnComponentEndOverlap.AddDynamic(this, &ABuilding::OnOverlapEnd_Building);*/

	(*SkeletalMeshComp)->SetGenerateOverlapEvents(true);
	(*SkeletalMeshComp)->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	(*SkeletalMeshComp)->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	(*SkeletalMeshComp)->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	//// (�⺻������ true�� �Ǿ� ����) NavMesh�� ������Ʈ �ǵ��� CanEverAffectNavigation�� true�� ����.
	////StaticMeshComp->SetCanEverAffectNavigation(true);

	// static Ű���带 �����Ͽ� �ν��Ͻ����� �ּ�(���ҽ�)�� �� �� ���� �ε��մϴ�. (����: static�� ������ �� ���� ���� �˴ϴ�.)
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshComp(ObjectToFind);
	if (skeletalMeshComp.Succeeded())
	{
		(*SkeletalMeshComp)->SetSkeletalMesh(skeletalMeshComp.Object);

		// StaticMesh�� ���� ������ ����
		FVector minBounds, maxBounds;
		(*SubStaticMeshComp)->GetLocalBounds(minBounds, maxBounds);
		//UE_LOG(LogTemp, Warning, TEXT("%s minBounds: %f, %f, %f"), *(*SkeletalMeshComp)->GetFName().ToString(), minBounds.X, minBounds.Y, minBounds.Z);
		//UE_LOG(LogTemp, Warning, TEXT("%s maxBounds: %f, %f, %f"), *(*SkeletalMeshComp)->GetFName().ToString(), maxBounds.X, maxBounds.Y, maxBounds.Z);

		// RootComponent�� SphereComponent�� StaticMesh�� �ϴ� ���߾����� ���Բ� ��������� ��.
		// ������ S->R->T ������ �ؾ� �������� ����� ����.
		(*SkeletalMeshComp)->SetRelativeScale3D(Scale);
		(*SkeletalMeshComp)->SetRelativeRotation(Rotation);
		FVector center;
		center.X = -1.0f * ((maxBounds.X * Scale.X + minBounds.X * Scale.X) / 2.0f);
		center.Y = -1.0f * ((maxBounds.Y * Scale.Y + minBounds.Y * Scale.Y) / 2.0f);
		center.Z = -1.0f * (minBounds.Z * Scale.Z);
		(*SkeletalMeshComp)->SetRelativeLocation(center + Location);

		// ���� ���͸��� ����
		FTArrayOfUMaterialInterface TArrayOfUMaterialInterface;
		TArrayOfUMaterialInterface.Object = (*SkeletalMeshComp)->GetMaterials();
		BuildingSkMCsMaterials.Add(TArrayOfUMaterialInterface);

		//UE_LOG(LogTemp, Warning, TEXT("%s center: %f, %f, %f"), *(*SkeletalMeshComp)->GetFName().ToString(), center.X, center.Y, center.Z);
	}

	BuildingSkMCs.Add(*SkeletalMeshComp);

}


void ABuilding::OnOverlapBegin_ConstructBuilding(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// ����ü ���� (�浹 ��ü�� AProjectile�� �ڵ忡�� ó���� ��)
	if (OtherActor->IsA(AProjectile::StaticClass()))
		return;
}

void ABuilding::OnOverlapBegin_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// ����ü ���� (�浹 ��ü�� AProjectile�� �ڵ忡�� ó���� ��)
	if (OtherActor->IsA(AProjectile::StaticClass()))
		return;

	// �ڱ� �ڽŰ� �浹�ϸ� �����մϴ�.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherComp->IsA(UWidgetComponent::StaticClass()))
		return;

	if (BuildingState == EBuildingState::Constructable)
	{
		//if (OtherActor->IsA(ABaseCharacter::StaticClass()))
		//{
		//	if (ABaseCharacter* baseCharacter = dynamic_cast<ABaseCharacter*>(OtherActor))
		//	{
		//		// ���� OtherActor�� ABaseCharacter�̱�� ������ DetactRangeSphereComp�� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
		//		if (baseCharacter->DetactRangeSphereComp == OtherComp || baseCharacter->AttackRangeSphereComp == OtherComp)
		//			return;
		//	}
		//}

		// �浹�� ������ OtherComp�� SphereComponent��� ����
		if (OtherComp->IsA(USphereComponent::StaticClass()))
			return;

		// �浹�� ������ OtherComp�� UCapsuleComponent(��ǻ� SkeletalMeshComponent)�� UStaticMeshComponent�� �߰�
		if (OtherComp->IsA(UCapsuleComponent::StaticClass()) ||
			OtherComp->IsA(UStaticMeshComponent::StaticClass()))
		{
			OverapedActors.Add(OtherActor);
		}
		// StaticMesh ��ü�� ���ʹ� AStaticMeshActor���� UStaticMesh���� Ȯ���غ��� ��.
		else if (OtherActor->IsA(AStaticMeshActor::StaticClass()) ||
			OtherActor->IsA(UStaticMesh::StaticClass()))
		{
			OverapedActors.Add(OtherActor);
		}
	}
}

void ABuilding::OnOverlapEnd_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// ����ü ���� (�浹 ��ü�� AProjectile�� �ڵ忡�� ó���� ��)
	if (OtherActor->IsA(AProjectile::StaticClass()))
		return;

	// �ڱ� �ڽŰ� �浹�ϸ� �����մϴ�.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (BuildingState == EBuildingState::Constructable)
		OverapedActors.RemoveSingle(OtherActor);
}

void ABuilding::TickOfConsumeAndProduct(float DeltaTime)
{
	TimerOfTickOfConsumeAndProduct += DeltaTime;
	if (TimerOfTickOfConsumeAndProduct < 1.0f)
		return;
	TimerOfTickOfConsumeAndProduct -= 1.0f;

	if (BuildingState == EBuildingState::Constructing)
	{
		HealthPoint += TickHealthPoint;
		if (HealthPoint > MaxHealthPoint)
			HealthPoint = MaxHealthPoint;
	}
	// �Ǽ��� �Ϸ�� ��쿡�� �����մϴ�.
	else if (BuildingState == EBuildingState::Constructed)
	{
		// ����� ������ �Һ� ���º��� ������ ������ �� �ֽ��ϴ�.
		if (APioneerManager::Resources.NumOfEnergy >= ConsumeElectricPower)
		{
			APioneerManager::Resources.NumOfMineral += ProductionMineral;
			APioneerManager::Resources.NumOfOrganic += ProductionOrganicMatter;
			APioneerManager::Resources.NumOfEnergy -= ConsumeElectricPower;
		}

		// ����� �ڿ��� �Һ� �ڿ����� ������ ���⸦ ������ �� �ֽ��ϴ�.
		if (APioneerManager::Resources.NumOfMineral >= ConsumeMineral
			&& APioneerManager::Resources.NumOfOrganic >= ConsumeOrganicMatter)
		{
			APioneerManager::Resources.NumOfMineral -= ConsumeMineral;
			APioneerManager::Resources.NumOfOrganic -= ConsumeOrganicMatter;
			APioneerManager::Resources.NumOfEnergy += ProductionElectricPower;
		}
	}
}

void ABuilding::SetHealthPoint(float Value)
{
	HealthPoint += Value;

	if (HealthPoint > 0.0f)
		return;

	BuildingState = EBuildingState::Destroying;

	if (BuildingManager)
	{
		if (BuildingManager->Buildings.Contains(ID))
		{
			BuildingManager->Buildings.Remove(ID);
		}
	}

	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			ServerSocketInGame->SendDestroyBuilding(ID);
		}
	}

	Destroy();
}


void ABuilding::SetBuildingMaterials()
{
	for (int i = 0; i < BuildingSMCs.Num(); i++)
	{
		for (int mat = 0; mat < BuildingSMCsMaterials[i].Object.Num(); mat++)
		{
			BuildingSMCs[i]->SetMaterial(mat, BuildingSMCsMaterials[i].Object[mat]);
		}
	}
	for (int i = 0; i < BuildingSkMCs.Num(); i++)
	{
		for (int mat = 0; mat < BuildingSkMCsMaterials[i].Object.Num(); mat++)
		{
			BuildingSkMCs[i]->SetMaterial(mat, BuildingSkMCsMaterials[i].Object[mat]);
		}
	}
}

void ABuilding::SetConstructableMaterial()
{
	if (!ConstructableMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABuilding::SetConstructableMaterial(): !ConstructableMaterial"));
		return;
	}
	for (auto& BuildingSMC : BuildingSMCs)
	{
		for (int mat = 0; mat < BuildingSMC->GetNumMaterials(); mat++)
		{
			BuildingSMC->SetMaterial(mat, ConstructableMaterial);
		}
	}
	for (auto& BuildingSkMC : BuildingSkMCs)
	{
		for (int mat = 0; mat < BuildingSkMC->GetNumMaterials(); mat++)
		{
			BuildingSkMC->SetMaterial(mat, ConstructableMaterial);
		}
	}
}

void ABuilding::SetUnConstructableMaterial()
{
	if (!UnConstructableMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABuilding::SetUnConstructableMaterial(): !UnConstructableMaterial"));
		return;
	}
	for (auto& BuildingSMC : BuildingSMCs)
	{
		for (int mat = 0; mat < BuildingSMC->GetNumMaterials(); mat++)
		{
			BuildingSMC->SetMaterial(mat, UnConstructableMaterial);
		}
	}
	for (auto& BuildingSkMC : BuildingSkMCs)
	{
		for (int mat = 0; mat < BuildingSkMC->GetNumMaterials(); mat++)
		{
			BuildingSkMC->SetMaterial(mat, UnConstructableMaterial);
		}
	}
}


void ABuilding::Rotating(float Value)
{
	FRotator rot = RootComponent->RelativeRotation;
	rot.Yaw += Value;

	if (rot.Yaw < 0.0f)
		rot.Yaw += 360.0f;
	else if (rot.Yaw > 360.0f)
		rot.Yaw -= 360.0f;

	RootComponent->SetRelativeRotation(rot);
}


bool ABuilding::Constructing()
{
	if (OverapedActors.Num() > 0)
		return false;


	// Constructable --> Constructing
	BuildingState = EBuildingState::Constructing;

	// Constructable Building�� ��Ȱ��ȭ
	for (auto& BuildingSMC : BuildingSMCs)
	{
		if (BuildingSMC)
		{
			BuildingSMC->SetGenerateOverlapEvents(false);
			BuildingSMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			BuildingSMC->SetHiddenInGame(true);
		}
	}
	for (auto& BuildingSkMC : BuildingSkMCs)
	{
		if (BuildingSkMC)
		{
			BuildingSkMC->SetGenerateOverlapEvents(false);
			BuildingSkMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			BuildingSkMC->SetHiddenInGame(true);
		}
	}

	// Constructing Building�� Ȱ��ȭ
	for (auto& ConstructBuildingSMC : ConstructBuildingSMCs)
	{
		if (ConstructBuildingSMC)
		{
			ConstructBuildingSMC->SetGenerateOverlapEvents(true);
			ConstructBuildingSMC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			ConstructBuildingSMC->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
			ConstructBuildingSMC->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

			ConstructBuildingSMC->SetHiddenInGame(false);
		}
	}

	// CheckConstructable()���� �����ߴٸ� Timer�� �����մϴ�.
	if (GetWorldTimerManager().IsTimerActive(TimerOfConstructing))
		GetWorldTimerManager().ClearTimer(TimerOfConstructing);

	// �Ǽ��ϼ� Ÿ�̸Ӹ� �����մϴ�.
	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &ABuilding::CompleteConstructing, ConstructionTime, false);

	return true;
}

void ABuilding::CheckConstructable()
{
	if (GetWorldTimerManager().IsTimerActive(TimerOfConstructing))
		GetWorldTimerManager().ClearTimer(TimerOfConstructing);

	// ���ļ� �Ǽ��� �� ������ ��� üũ�մϴ�.
	if (Constructing() == false)
	{
		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("Constructing"));
		GetWorldTimerManager().SetTimer(TimerOfConstructing, timerDel, 0.5f, true);
	}
}

void ABuilding::CompleteConstructing()
{
	// Constructing --> Constructed
	BuildingState = EBuildingState::Constructed;

	// Constructing Building�� �Ҹ�
	for (auto& ConstructBuildingSMC : ConstructBuildingSMCs)
	{
		if (ConstructBuildingSMC)
		{
			ConstructBuildingSMC->DestroyComponent();
		}
	}
	ConstructBuildingSMCs.Reset();

	// Constructed Building�� Ȱ��ȭ
	for (auto& BuildingSMC : BuildingSMCs)
	{
		if (BuildingSMC)
		{
			BuildingSMC->SetGenerateOverlapEvents(true);
			BuildingSMC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			BuildingSMC->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
			BuildingSMC->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

			BuildingSMC->SetHiddenInGame(false);
		}
	}
	for (auto& BuildingSkMC : BuildingSkMCs)
	{
		if (BuildingSkMC)
		{
			BuildingSkMC->SetHiddenInGame(false);
		}
	}

	SetBuildingMaterials();

	HealthPoint = MaxHealthPoint;
}

void ABuilding::Destroying()
{
	Destroy();
}

void ABuilding::SetInfoOfBuilding_Spawn(class cInfoOfBuilding_Spawn& Spawn)
{
	ID = Spawn.ID;

	BuildingType = (EBuildingType)Spawn.Numbering;

	NeedMineral = Spawn.NeedMineral;
	NeedOrganicMatter = Spawn.NeedOrganicMatter;

	SetActorTransform(Spawn.GetActorTransform());
}
class cInfoOfBuilding_Spawn ABuilding::GetInfoOfBuilding_Spawn()
{
	cInfoOfBuilding_Spawn infoOfBuilding_Spawn;

	infoOfBuilding_Spawn.ID = ID;

	infoOfBuilding_Spawn.Numbering = (int)BuildingType;

	infoOfBuilding_Spawn.NeedMineral = NeedMineral;
	infoOfBuilding_Spawn.NeedOrganicMatter = NeedOrganicMatter;

	infoOfBuilding_Spawn.SetActorTransform(GetActorTransform());

	return infoOfBuilding_Spawn;
}

void ABuilding::SetInfoOfBuilding_Stat(class cInfoOfBuilding_Stat& Stat)
{
	
	EBuildingState newBuildingState = (EBuildingState)Stat.BuildingState;

	if (BuildingState != EBuildingState::Constructed && newBuildingState == EBuildingState::Constructed)
	{
		CompleteConstructing();
	}

	BuildingState = newBuildingState;

	HealthPoint = Stat.HealthPoint;

	SetHealthPoint(NULL);
}
class cInfoOfBuilding_Stat ABuilding::GetInfoOfBuilding_Stat()
{
	cInfoOfBuilding_Stat Stat;

	Stat.ID = ID;

	Stat.BuildingState = (int)BuildingState;

	Stat.HealthPoint = HealthPoint;

	return Stat;
}

void ABuilding::SetInfoOfBuilding(class cInfoOfBuilding& InfoOfBuilding)
{
	SetInfoOfBuilding_Spawn(InfoOfBuilding.Spawn);
	SetInfoOfBuilding_Stat(InfoOfBuilding.Stat);
}
class cInfoOfBuilding ABuilding::GetInfoOfBuilding()
{
	cInfoOfBuilding infoOfBuilding(ID, GetInfoOfBuilding_Spawn(), GetInfoOfBuilding_Stat());

	return infoOfBuilding;
}
/*** ABuilding : End ***/

