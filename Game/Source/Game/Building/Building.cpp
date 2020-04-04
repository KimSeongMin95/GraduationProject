// Fill out your copyright notice in the Description page of Project Settings.

#include "Building.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/Projectile.h"
#include "Character/BaseCharacter.h"

#include "Network/Packet.h"
#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "PioneerManager.h"

#include "BuildingManager.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ABuilding::ABuilding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->SetGenerateOverlapEvents(false);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

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


	// Pioneer가 생성한게 아니라면 건물을 바로 완성시킴
	if (!GetOwner())
	{
		// 게임클라이언트라면 게임서버에서 SpawnBuilding으로 생성하기 때문에 소멸시킵니다.
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
		if (OverlappedActors.Num() > 0)
			SetUnConstructableMaterial();
		else
			SetConstructableMaterial();
	}

	TickHelthPointBar();

	TickOfConsumeAndProduct(DeltaTime);

	//// 임시
	//SetHealthPoint(NULL);
}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ABuilding::InitHelthPointBar()
{
	HelthPointBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HelthPointBar"));
	//HelthPointBar = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass());
	HelthPointBar->SetupAttachment(RootComponent);
	HelthPointBar->bAbsoluteRotation = true; // 절대적인 회전값을 적용합니다.

	HelthPointBar->SetGenerateOverlapEvents(false);
	HelthPointBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HelthPointBar->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
	HelthPointBar->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	HelthPointBar->SetOnlyOwnerSee(false);
	//HelthPointBar->SetIsReplicated(false);

	HelthPointBar->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	//HelthPointBar->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	HelthPointBar->SetRelativeRotation(FRotator(45.0f, 180.0f, 0.0f)); // 항상 플레이어에게 보이도록 회전 값을 World로 해야 함.
	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 30));

	// Screen은 뷰포트에서 UI처럼 띄워주는 것이고 World는 게임 내에서 UI처럼 띄워주는 것
	HelthPointBar->SetWidgetSpace(EWidgetSpace::World);
}
void ABuilding::BeginPlayHelthPointBar()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!world)"));
#endif
		return;
	}

	/*** 주의: Blueprint 애셋은 뒤에 _C를 붙여줘서 클래스를 가져와줘야 함. ***/
	FString HelthPointBarBP_Reference = "WidgetBlueprint'/Game/Characters/HelthPointBar.HelthPointBar_C'";
	UClass* HelthPointBarBP = LoadObject<UClass>(this, *HelthPointBarBP_Reference);

	// 가져온 WidgetBlueprint를 UWidgetComponent에 바로 적용하지말고 따로 UUserWidget에 저장하여 설정을 한 뒤
	// UWidgetComponent->SetWidget(저장한 UUserWidget);으로 UWidgetComponent에 적용해야 함.
	//HelthPointBar->SetWidgetClass(HelthPointBarBP);
	HelthPointBarUserWidget = CreateWidget(world, HelthPointBarBP); // wolrd가 꼭 필요.

	if (HelthPointBarUserWidget)
	{
		UWidgetTree* WidgetTree = HelthPointBarUserWidget->WidgetTree;
		if (WidgetTree)
		{
			//// 이 방법은 안됨.
			// ProgreeBar = Cast<UProgressBar>(HelthPointBarUserWidget->GetWidgetFromName(FName(TEXT("ProgressBar_153"))));

			ProgressBar = WidgetTree->FindWidget<UProgressBar>(FName(TEXT("ProgressBar_153")));
			if (!ProgressBar)
			{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
				UE_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!ProgressBar)"));
#endif
			}
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!WidgetTree)"));
#endif
		}
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!HelthPointBarUserWidget)"));
#endif
	}

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
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void ABuilding::InitBuilding()
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
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

	(*StaticMeshComp)->SetGenerateOverlapEvents(false);
	(*StaticMeshComp)->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	(*StaticMeshComp)->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	(*StaticMeshComp)->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);
	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);

	(*StaticMeshComp)->SetHiddenInGame(true);

	// static 키워드를 제거하여 인스턴스마다 애셋(리소스)를 매 번 새로 로드합니다. (주의: static이 붙으면 다 같은 모델이 됩니다.)
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(ObjectToFind);
	if (staticMesh.Succeeded())
	{
		(*StaticMeshComp)->SetStaticMesh(staticMesh.Object);

		// StaticMesh의 원본 사이즈 측정
		FVector minBounds, maxBounds;
		(*StaticMeshComp)->GetLocalBounds(minBounds, maxBounds);

		// RootComponent인 SphereComponent가 StaticMesh의 하단 정중앙으로 오게끔 설정해줘야 함.
		// 순서는 S->R->T 순으로 해야 원점에서 벗어나지 않음.
		(*StaticMeshComp)->SetRelativeScale3D(Scale);
		(*StaticMeshComp)->SetRelativeRotation(Rotation);
		FVector center;
		center.X = -1.0f * ((maxBounds.X * Scale.X + minBounds.X * Scale.X) / 2.0f);
		center.Y = -1.0f * ((maxBounds.Y * Scale.Y + minBounds.Y * Scale.Y) / 2.0f);
		center.Z = -1.0f * (minBounds.Z * Scale.Z);
		(*StaticMeshComp)->SetRelativeLocation(center + Location);

//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		//UE_LOG(LogTemp, Log, TEXT("%s minBounds: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), minBounds.X, minBounds.Y, minBounds.Z);
		//UE_LOG(LogTemp, Log, TEXT("%s maxBounds: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), maxBounds.X, maxBounds.Y, maxBounds.Z);
		//UE_LOG(LogTemp, Log, TEXT("%s center: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), center.X, center.Y, center.Z);
//#endif
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
	(*StaticMeshComp)->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	(*StaticMeshComp)->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);
	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
	(*StaticMeshComp)->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);

	// (기본적으로 true로 되어 있음) NavMesh에 업데이트 되도록 CanEverAffectNavigation을 true로 변경.
	//StaticMeshComp->SetCanEverAffectNavigation(true);

	// static 키워드를 제거하여 인스턴스마다 애셋(리소스)를 매 번 새로 로드합니다. (주의: static이 붙으면 다 같은 모델이 됩니다.)
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshComp(ObjectToFind);
	if (staticMeshComp.Succeeded())
	{
		(*StaticMeshComp)->SetStaticMesh(staticMeshComp.Object);

		// StaticMesh의 원본 사이즈 측정
		FVector minBounds, maxBounds;
		(*StaticMeshComp)->GetLocalBounds(minBounds, maxBounds);

		// RootComponent인 SphereComponent가 StaticMesh의 하단 정중앙으로 오게끔 설정해줘야 함.
		// 순서는 S->R->T 순으로 해야 원점에서 벗어나지 않음.
		(*StaticMeshComp)->SetRelativeScale3D(Scale);
		(*StaticMeshComp)->SetRelativeRotation(Rotation);
		FVector center;
		center.X = -1.0f * ((maxBounds.X * Scale.X + minBounds.X * Scale.X) / 2.0f);
		center.Y = -1.0f * ((maxBounds.Y * Scale.Y + minBounds.Y * Scale.Y) / 2.0f);
		center.Z = -1.0f * (minBounds.Z * Scale.Z);
		(*StaticMeshComp)->SetRelativeLocation(center + Location);

		// 원본 머터리얼 저장
		FTArrayOfUMaterialInterface TArrayOfUMaterialInterface;
		TArrayOfUMaterialInterface.Object = (*StaticMeshComp)->GetMaterials();
		BuildingSMCsMaterials.Add(TArrayOfUMaterialInterface);

//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		//UE_LOG(LogTemp, Log, TEXT("%s minBounds: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), minBounds.X, minBounds.Y, minBounds.Z);
		//UE_LOG(LogTemp, Log, TEXT("%s maxBounds: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), maxBounds.X, maxBounds.Y, maxBounds.Z);
		//UE_LOG(LogTemp, Log, TEXT("%s center: %f, %f, %f"), *(*StaticMeshComp)->GetFName().ToString(), center.X, center.Y, center.Z);
//#endif	
	}

	BuildingSMCs.Add(*StaticMeshComp);

}

void ABuilding::AddBuildingSkMC(USkeletalMeshComponent** SkeletalMeshComp,
	const TCHAR* CompName, const TCHAR* ObjectToFind,
	FVector Scale, FRotator Rotation, FVector Location)
{
	(*SkeletalMeshComp) = CreateDefaultSubobject<USkeletalMeshComponent>(CompName);
	(*SkeletalMeshComp)->SetupAttachment(RootComponent);
	(*SkeletalMeshComp)->SetVisibility(true);

	/*(*SkeletalMeshComp)->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin_Building);
	(*SkeletalMeshComp)->OnComponentEndOverlap.AddDynamic(this, &ABuilding::OnOverlapEnd_Building);*/

	(*SkeletalMeshComp)->SetGenerateOverlapEvents(false);
	(*SkeletalMeshComp)->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	(*SkeletalMeshComp)->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	(*SkeletalMeshComp)->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	//// (기본적으로 true로 되어 있음) NavMesh에 업데이트 되도록 CanEverAffectNavigation을 true로 변경.
	////StaticMeshComp->SetCanEverAffectNavigation(true);

	// static 키워드를 제거하여 인스턴스마다 애셋(리소스)를 매 번 새로 로드합니다. (주의: static이 붙으면 다 같은 모델이 됩니다.)
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshComp(ObjectToFind);
	if (skeletalMeshComp.Succeeded())
	{
		(*SkeletalMeshComp)->SetSkeletalMesh(skeletalMeshComp.Object);

		(*SkeletalMeshComp)->SetRelativeScale3D(Scale);
		(*SkeletalMeshComp)->SetRelativeRotation(Rotation);
		(*SkeletalMeshComp)->SetRelativeLocation(Location);

		// 원본 머터리얼 저장
		FTArrayOfUMaterialInterface TArrayOfUMaterialInterface;
		TArrayOfUMaterialInterface.Object = (*SkeletalMeshComp)->GetMaterials();
		BuildingSkMCsMaterials.Add(TArrayOfUMaterialInterface);
	}

	BuildingSkMCs.Add(*SkeletalMeshComp);

}


void ABuilding::OnOverlapBegin_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	//UE_LOG(LogTemp, Log, TEXT("<ABuilding::OnOverlapBegin_Building(...)> Character FName: %s"), *OtherActor->GetFName().ToString());
//#endif
 
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	if (BuildingState != EBuildingState::Constructable)
		return;

	/**************************************************/

	if (OtherActor->IsA(ABaseCharacter::StaticClass()))
	{
		if (ABaseCharacter* baseCharacter = dynamic_cast<ABaseCharacter*>(OtherActor))
		{
			if (baseCharacter->GetCapsuleComponent() == OtherComp)
			{
				OverlappedActors.Add(OtherActor);
			}
		}
	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		OverlappedActors.Add(OtherActor);
	}
	else if (OtherActor->IsA(AStaticMeshActor::StaticClass()))
	{
		OverlappedActors.Add(OtherActor);
	}

	//// 충돌한 액터의 OtherComp가 UCapsuleComponent(사실상 SkeletalMeshComponent)나 UStaticMeshComponent면 추가
	//else if (OtherComp->IsA(UCapsuleComponent::StaticClass()) ||
	//	OtherComp->IsA(UStaticMeshComponent::StaticClass()))
	//{
	//	OverlappedActors.Add(OtherActor);
	//}
		
	//// StaticMesh 자체인 액터는 AStaticMeshActor인지 UStaticMesh인지 확인해봐야 함.
	//if (OtherActor->IsA(AStaticMeshActor::StaticClass()) ||
	//	OtherActor->IsA(UStaticMesh::StaticClass()))
	//{
	//	OverlappedActors.Add(OtherActor);
	//}
}

void ABuilding::OnOverlapEnd_Building(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	if (BuildingState != EBuildingState::Constructable)
		return;

	/**************************************************/

	if (OtherActor->IsA(ABaseCharacter::StaticClass()))
	{
		if (ABaseCharacter* baseCharacter = dynamic_cast<ABaseCharacter*>(OtherActor))
		{
			if (baseCharacter->GetCapsuleComponent() == OtherComp)
			{
				OverlappedActors.RemoveSingle(OtherActor);
			}
		}
	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		OverlappedActors.RemoveSingle(OtherActor);
	}
	else if (OtherActor->IsA(AStaticMeshActor::StaticClass()))
	{
		OverlappedActors.RemoveSingle(OtherActor);
	}
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
	// 건설이 완료된 경우에만 실행합니다.
	else if (BuildingState == EBuildingState::Constructed)
	{
		// 저장된 전력이 소비 전력보다 많으면 생산할 수 있습니다.
		if (APioneerManager::Resources.NumOfEnergy >= ConsumeElectricPower)
		{
			APioneerManager::Resources.NumOfMineral += ProductionMineral;
			APioneerManager::Resources.NumOfOrganic += ProductionOrganicMatter;
			APioneerManager::Resources.NumOfEnergy -= ConsumeElectricPower;
		}

		// 저장된 자원이 소비 자원보다 많으면 전기를 생산할 수 있습니다.
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

	if (BuildingState == EBuildingState::Destroying)
		return;

	/************************************/

	if (!BuildingManager)
	{
		UE_LOG(LogTemp, Fatal, TEXT("<ABuilding::SetHealthPoint(...)> if (!BuildingManager)"));
	}

	BuildingState = EBuildingState::Destroying;

	if (BuildingManager)
	{
		if (BuildingManager->Buildings.Contains(ID))
		{
			BuildingManager->Buildings[ID] = nullptr;
			BuildingManager->Buildings.Remove(ID);
			BuildingManager->Buildings.Compact();
			BuildingManager->Buildings.Shrink();
		}
		else
		{
			UE_LOG(LogTemp, Fatal, TEXT("<ABuilding::SetHealthPoint(...)> if (!BuildingManager->Buildings.Contains(ID))"));
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<ABuilding::SetConstructableMaterial()> if (!ConstructableMaterial)"));
#endif
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<ABuilding::SetUnConstructableMaterial()> if (!UnConstructableMaterial)"));
#endif
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
	if (OverlappedActors.Num() > 0)
		return false;


	// Constructable --> Constructing
	BuildingState = EBuildingState::Constructing;

	// Constructable Building들 비활성화
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

	// Constructing Building들 활성화
	for (auto& ConstructBuildingSMC : ConstructBuildingSMCs)
	{
		if (ConstructBuildingSMC)
		{
			ConstructBuildingSMC->SetGenerateOverlapEvents(true);
			ConstructBuildingSMC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			ConstructBuildingSMC->SetHiddenInGame(false);
		}
	}

	// CheckConstructable()으로 실행했다면 Timer를 종료합니다.
	if (GetWorldTimerManager().IsTimerActive(TimerOfConstructing))
		GetWorldTimerManager().ClearTimer(TimerOfConstructing);

	// 건설완성 타이머를 실행합니다.
	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &ABuilding::CompleteConstructing, ConstructionTime, false);

	return true;
}

void ABuilding::CheckConstructable()
{
	if (GetWorldTimerManager().IsTimerActive(TimerOfConstructing))
		GetWorldTimerManager().ClearTimer(TimerOfConstructing);

	// 겹쳐서 건설할 수 없으면 계속 체크합니다.
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

	// Constructing Building들 소멸
	for (auto& ConstructBuildingSMC : ConstructBuildingSMCs)
	{
		if (ConstructBuildingSMC)
		{
			ConstructBuildingSMC->DestroyComponent();
		}
	}
	ConstructBuildingSMCs.Reset();

	// Constructed Building들 활성화
	for (auto& BuildingSMC : BuildingSMCs)
	{
		if (BuildingSMC)
		{
			BuildingSMC->SetGenerateOverlapEvents(true);
			BuildingSMC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			BuildingSMC->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			BuildingSMC->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

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

