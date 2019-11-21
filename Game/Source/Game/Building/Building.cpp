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

	/*** RootComponent : Start ***/
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->SetGenerateOverlapEvents(false);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SphereComponent;
	/*** RootComponent : End ***/

	InitStatement();

	InitConstructBuildingSMC();

	InitBuildingSMC();

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

/*** Statements : Start ***/
void ABuilding::InitStatement()
{
	// Default Settings
	HP = 10.0f;
	CompleteHP = 100.0f;

	Size = FVector2D(1.0f, 1.0f);
	ConstructionTime = 3.0f;

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
void ABuilding::OnOverlapBegin_ConstructBuildingSMC(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ABuilding::InitConstructBuildingSMC()
{
	ConstructBuildingSMC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConstructBuildingStaticMeshComponent"));
	ConstructBuildingSMC->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> constructBuildingSMC(TEXT("StaticMesh'/Game/Buildings/InorganicMine/Temp_InorganicMine.Temp_InorganicMine'"));
	if (constructBuildingSMC.Succeeded())
	{
		ConstructBuildingSMC->SetStaticMesh(constructBuildingSMC.Object);

		ConstructBuildingSMC->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin_ConstructBuildingSMC);
		ConstructBuildingSMC->SetGenerateOverlapEvents(false);
		ConstructBuildingSMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ConstructBuildingSMC->SetHiddenInGame(true);
	}
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void ABuilding::SetBuildingSMCMaterials()
{
	int num = BuildingSMCMaterials.Num();
	for (int i = 0; i < num; i++)
	{
		BuildingSMC->SetMaterial(i, BuildingSMCMaterials[i]);
	}
}

void ABuilding::OnOverlapBegin_BuildingSMC(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ABuilding::OnOverlapEnd_BuildingSMC(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ABuilding::InitBuildingSMC()
{
	bIsConstructing = false;
	bCompleted = false;

	BuildingSMC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingStaticMeshComponent"));
	BuildingSMC->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> buildingSMC(TEXT("StaticMesh'/Game/Buildings/Walls/SM_Intersection_Wall.SM_Intersection_Wall'"));
	if (buildingSMC.Succeeded())
	{
		BuildingSMC->SetStaticMesh(buildingSMC.Object);
		BuildingSMCMaterials = BuildingSMC->GetMaterials();

		BuildingSMC->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin_BuildingSMC);
		BuildingSMC->OnComponentEndOverlap.AddDynamic(this, &ABuilding::OnOverlapEnd_BuildingSMC);

		BuildingSMC->SetGenerateOverlapEvents(true);
		BuildingSMC->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BuildingSMC->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		BuildingSMC->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	}
}
/*** BuildingStaticMeshComponent : End ***/

/*** Material : Start ***/
void ABuilding::SetConstructableMaterial()
{
	if (!ConstructableMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABuilding::SetConstructableMaterial(): !ConstructableMaterial"));
		return;
	}

	for (int i = 0; i < BuildingSMC->GetNumMaterials(); i++)
	{
		BuildingSMC->SetMaterial(i, ConstructableMaterial);
	}
}

void ABuilding::SetUnConstructableMaterial()
{
	if (!UnConstructableMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABuilding::SetUnConstructableMaterial(): !UnConstructableMaterial"));
		return;
	}

	for (int i = 0; i < BuildingSMC->GetNumMaterials(); i++)
	{
		BuildingSMC->SetMaterial(i, UnConstructableMaterial);
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
	
	if (BuildingSMC)
	{
		BuildingSMC->SetGenerateOverlapEvents(false);
		BuildingSMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		BuildingSMC->SetHiddenInGame(true);
	}

	if (ConstructBuildingSMC)
	{
		ConstructBuildingSMC->SetGenerateOverlapEvents(true);
		ConstructBuildingSMC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ConstructBuildingSMC->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
		ConstructBuildingSMC->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	
		ConstructBuildingSMC->SetHiddenInGame(false);
	}

	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &ABuilding::CompleteConstructing, ConstructionTime, false);

	return true;
}
void ABuilding::Destorying()
{
	Destroy();
}
void ABuilding::CompleteConstructing()
{
	bIsConstructing = false;
	bCompleted = true;

	if (BuildingSMC)
	{
		BuildingSMC->SetGenerateOverlapEvents(true);
		BuildingSMC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BuildingSMC->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
		BuildingSMC->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

		BuildingSMC->SetHiddenInGame(false);

		
	}

	if (ConstructBuildingSMC)
	{
		ConstructBuildingSMC->SetGenerateOverlapEvents(false);
		ConstructBuildingSMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ConstructBuildingSMC->SetHiddenInGame(true);
	}

	SetBuildingSMCMaterials();
}
/*** Constructing And Destorying : End ***/



