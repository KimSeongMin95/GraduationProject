// Fill out your copyright notice in the Description page of Project Settings.

#include "Building.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/Projectile.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
ABuilding::ABuilding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitRootComponent();

	InitStatement();

	InitConstructBuildingSMCs();

	InitBuildingSMCs();

	InitMaterial();
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bCompleted)
	{
		if (!bIsConstructing && OverapedActors.Num() > 0)
		{
			SetUnConstructableMaterial();
		}
		else
		{
			SetConstructableMaterial();
		}
	}
}

/*** RootComponent : Start ***/
void ABuilding::InitRootComponent()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->SetGenerateOverlapEvents(false);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SphereComponent;
}
/*** RootComponent : End ***/

/*** Statements : Start ***/
void ABuilding::InitStatement()
{
	// Default Settings
	HP = 10.0f;
	CompleteHP = 100.0f;

	Size = FVector2D(1.0f, 1.0f);
	ConstructionTime = 2.0f;

	NeedMineral = 0.0f;
	NeedOrganicMatter = 0.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 0.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}
/*** Statements : End ***/

/*** ConstructBuildingStaticMeshComponent : Start ***/
void ABuilding::OnOverlapBegin_ConstructBuildingSMCs(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	// 투사체
	if (OtherActor->IsA(AProjectile::StaticClass()))
	{
		AProjectile* projectile = dynamic_cast<AProjectile*>(OtherActor);

		HP -= projectile->TotalDamage;
	}
}

void ABuilding::InitConstructBuildingSMCs()
{

}

void ABuilding::AddConstructBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale, FRotator Rotation, FVector Location)
{
	(*StaticMeshComp) = CreateDefaultSubobject<UStaticMeshComponent>(CompName);
	(*StaticMeshComp)->SetupAttachment(RootComponent);

	(*StaticMeshComp)->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin_ConstructBuildingSMCs);
	(*StaticMeshComp)->SetGenerateOverlapEvents(false);
	(*StaticMeshComp)->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	}

	ConstructBuildingSMCs.Add(*StaticMeshComp);
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void ABuilding::OnOverlapBegin_BuildingSMCs(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	if (!bIsConstructing)
	{
		// 투사체도 무시합니다.
		if (OtherActor->IsA(AProjectile::StaticClass()))
		{
			return;
		}

		OverapedActors.Add(OtherActor);
	}
	else
	{
		// 투사체
		if (OtherActor->IsA(AProjectile::StaticClass()))
		{
			AProjectile* projectile = dynamic_cast<AProjectile*>(OtherActor);

			HP -= projectile->TotalDamage;
		}
	}
}

void ABuilding::OnOverlapEnd_BuildingSMCs(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	// 투사체도 무시합니다.
	if (OtherActor->IsA(AProjectile::StaticClass()))
	{
		return;
	}

	if (bIsConstructing)
	{

	}
	else
	{
		/*int index = OverapedActors.Find(OtherActor);
		if (index >= 0)
		{
			OverapedActors.RemoveAt(index);
		}*/
		OverapedActors.Remove(OtherActor);
	}
}

void ABuilding::InitBuildingSMCs()
{
	bIsConstructing = false;
	bCompleted = false;
}

void ABuilding::AddBuildingSMC(UStaticMeshComponent** StaticMeshComp, const TCHAR* CompName, const TCHAR* ObjectToFind, FVector Scale, FRotator Rotation, FVector Location)
{
	(*StaticMeshComp) = CreateDefaultSubobject<UStaticMeshComponent>(CompName);
	(*StaticMeshComp)->SetupAttachment(RootComponent);
	(*StaticMeshComp)->SetVisibility(true);

	(*StaticMeshComp)->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin_BuildingSMCs);
	(*StaticMeshComp)->OnComponentEndOverlap.AddDynamic(this, &ABuilding::OnOverlapEnd_BuildingSMCs);

	(*StaticMeshComp)->SetGenerateOverlapEvents(true);
	(*StaticMeshComp)->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	(*StaticMeshComp)->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	(*StaticMeshComp)->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

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
		//UE_LOG(LogTemp, Warning, TEXT("b minBounds: %f, %f, %f"), minBounds.X, minBounds.Y, minBounds.Z);
		//UE_LOG(LogTemp, Warning, TEXT("b maxBounds: %f, %f, %f"), maxBounds.X, maxBounds.Y, maxBounds.Z);

		// RootComponent인 SphereComponent가 StaticMesh의 하단 정중앙으로 오게끔 설정해줘야 함.
		// 순서는 S->R->T 순으로 해야 원점에서 벗어나지 않음.
		(*StaticMeshComp)->SetRelativeScale3D(Scale);
		(*StaticMeshComp)->SetRelativeRotation(Rotation);
		FVector center;
		center.X = -1.0f * ((maxBounds.X * Scale.X + minBounds.X * Scale.X) / 2.0f);
		center.Y = -1.0f * ((maxBounds.Y * Scale.Y + minBounds.Y * Scale.Y) / 2.0f);
		center.Z = -1.0f * (minBounds.Z * Scale.Z);
		(*StaticMeshComp)->SetRelativeLocation(center + Location);

		//BuildingSMCsMaterials.Add((*StaticMeshComp)->GetMaterials());
	}

	BuildingSMCs.Add(*StaticMeshComp);

}

//void ABuilding::SetBuildingSMCsMaterials()
//{
//	for (int i = 0; i < BuildingSMCs.Num(); i++)
//	{
//		for (int mat = 0; mat < BuildingSMCsMaterials[i].Num(); mat++)
//		{
//			BuildingSMCs[i]->SetMaterial(mat, BuildingSMCsMaterials[i][mat]);
//		}
//	}
//}
/*** BuildingStaticMeshComponent : End ***/

/*** Material : Start ***/
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
/*** Material : End ***/

/*** Rotation : Start ***/
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
/*** Rotation : End ***/

/*** Constructing And Destorying : Start ***/
bool ABuilding::Constructing()
{
	if (OverapedActors.Num() > 0)
		return false;

	bIsConstructing = true;

	for (auto& BuildingSMC : BuildingSMCs)
	{
		if (BuildingSMC)
		{
			BuildingSMC->SetGenerateOverlapEvents(false);
			BuildingSMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			BuildingSMC->SetHiddenInGame(true);
		}
	}

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

	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &ABuilding::CompleteConstructing, ConstructionTime, false);

	return true;
}
void ABuilding::Destroying()
{
	Destroy();
}
void ABuilding::CompleteConstructing()
{
	bIsConstructing = false;
	bCompleted = true;

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
	for (auto& ConstructBuildingSMC : ConstructBuildingSMCs)
	{
		if (ConstructBuildingSMC)
		{
			ConstructBuildingSMC->SetGenerateOverlapEvents(false);
			ConstructBuildingSMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			ConstructBuildingSMC->SetHiddenInGame(true);
		}
	}

	//SetBuildingSMCsMaterials();
}
/*** Constructing And Destorying : End ***/