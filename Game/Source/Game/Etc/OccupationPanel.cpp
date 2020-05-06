// Fill out your copyright notice in the Description page of Project Settings.


#include "OccupationPanel.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AOccupationPanel::AOccupationPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Occupancy = 0.0f;
	TimeOfOccupancy = 15.0f;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AOccupationPanel::OnOverlapBegin_Pioneer);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AOccupationPanel::OnOverlapEnd_Pioneer);
	SphereComponent->SetGenerateOverlapEvents(true);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel9);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComponent->SetSphereRadius(650.0f, true);


	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelMesh"));
	PanelMesh->SetupAttachment(RootComponent);
	PanelMesh->SetGenerateOverlapEvents(false);
	PanelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PanelMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	PanelMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ConstructorHelpers::FObjectFinder<UStaticMesh> panelMeshObject(TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularSci_Comm/Meshes/SM_Floor_Center_A3.SM_Floor_Center_A3'"));
	if (panelMeshObject.Succeeded())
	{
		PanelMesh->SetStaticMesh(panelMeshObject.Object);
	}

	ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> occupancyMaterialInstanceConstant(TEXT("MaterialInstanceConstant'/Game/ModularSciFiSeason1/ModularSci_Comm/Materials/MI_Trims_C_Blue.MI_Trims_C_Blue'"));
	if (occupancyMaterialInstanceConstant.Succeeded())
	{
		OccupancyMaterialInstanceConstant = occupancyMaterialInstanceConstant.Object;
	}


	DownBorderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DownBorderMesh"));
	DownBorderMesh->SetupAttachment(RootComponent);
	DownBorderMesh->SetGenerateOverlapEvents(false);
	DownBorderMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DownBorderMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	DownBorderMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ConstructorHelpers::FObjectFinder<UStaticMesh> downBorderMeshObject(TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularSci_Comm/Meshes/SM_Floor_Center_B.SM_Floor_Center_B'"));
	if (downBorderMeshObject.Succeeded())
	{
		DownBorderMesh->SetStaticMesh(downBorderMeshObject.Object);
	}
	DownBorderMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));


	InitHelthPointBar();

	Height = 512.0f;
	TimerOfUp = 0.0f;
	TimerOfFloating = 0.0f;
	TimerOfDown = 0.0f;
	DownBorderState = EDownBorderState::Start;

	TickFlag = false;
}

void AOccupationPanel::BeginPlay()
{
	Super::BeginPlay();
	
	BeginPlayHelthPointBar();
}

void AOccupationPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickHelthPointBar();

	TickOfOccupation(DeltaTime);

	TickOfDownBorderMesh(DeltaTime);
}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AOccupationPanel::InitHelthPointBar()
{
	HelthPointBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HelthPointBar"));
	//HelthPointBar = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass());
	HelthPointBar->SetupAttachment(DownBorderMesh);
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
	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 30.0f));
	HelthPointBar->SetDrawSize(FVector2D(256, 64));

	// Screen은 뷰포트에서 UI처럼 띄워주는 것이고 World는 게임 내에서 UI처럼 띄워주는 것
	HelthPointBar->SetWidgetSpace(EWidgetSpace::World);
}
void AOccupationPanel::BeginPlayHelthPointBar()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOccupationPanel::BeginPlayHelthPointBar()> if (!world)"));
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
				UE_LOG(LogTemp, Warning, TEXT("<AOccupationPanel::BeginPlayHelthPointBar()> if (!ProgressBar)"));
#endif
			}
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Warning, TEXT("<AOccupationPanel::BeginPlayHelthPointBar()> if (!WidgetTree)"));
#endif
		}
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<AOccupationPanel::BeginPlayHelthPointBar()> if (!HelthPointBarUserWidget)"));
#endif
	}

	HelthPointBar->SetWidget(HelthPointBarUserWidget);
}

void AOccupationPanel::TickHelthPointBar()
{
	if (!HelthPointBar)
		return;

	if (ProgressBar)
		ProgressBar->SetPercent(Occupancy / 100.0f);
}
/*** IHealthPointBarInterface : End ***/


/*** AOccupationPanel : Start ***/
void AOccupationPanel::OnOverlapBegin_Pioneer(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				OverlappedPioneers.Add(pioneer);
			}
		}
	}
}

void AOccupationPanel::OnOverlapEnd_Pioneer(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
				OverlappedPioneers.RemoveSingle(pioneer);
			}
		}
	}
}

void AOccupationPanel::TickOfOccupation(float DeltaTime)
{
	if (TickFlag)
		return;

	if (OverlappedPioneers.Num() >= 1)
	{
		Occupancy += (100.0f * DeltaTime) / TimeOfOccupancy;	

		if (DownBorderState == EDownBorderState::Start || DownBorderState == EDownBorderState::Down)
		{
			DownBorderState = EDownBorderState::Up;
			if (DownBorderMesh)
				TimerOfUp = FMath::Asin(DownBorderMesh->RelativeLocation.Z / Height) / PI;
		}
	}
	else
	{
		if (DownBorderState == EDownBorderState::Up || DownBorderState == EDownBorderState::Floating)
		{
			DownBorderState = EDownBorderState::Down;
			if (DownBorderMesh)
				TimerOfDown = DownBorderMesh->RelativeLocation.Z / Height;
		}
	}

	if (Occupancy >= 100.0f)
	{
		Occupancy = 100.0f;

		OverlappedPioneers.Reset();

		if (SphereComponent)
		{
			SphereComponent->OnComponentBeginOverlap.Clear();
			SphereComponent->OnComponentEndOverlap.Clear();
			SphereComponent->SetGenerateOverlapEvents(false);
			SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		}

		if (DownBorderState == EDownBorderState::Up || DownBorderState == EDownBorderState::Floating)
		{
			DownBorderState = EDownBorderState::Down;
			if (DownBorderMesh)
				TimerOfDown = DownBorderMesh->RelativeLocation.Z / Height;
		}

		if (PanelMesh)
		{
			PanelMesh->SetMaterial(0, OccupancyMaterialInstanceConstant);
			PanelMesh->SetMaterial(3, OccupancyMaterialInstanceConstant);
		}

		if (HelthPointBar)
		{
			HelthPointBar->DestroyComponent();
			HelthPointBar = nullptr;
		}

		TickFlag = true;
	}


}
void AOccupationPanel::TickOfDownBorderMesh(float DeltaTime)
{
	if (!DownBorderMesh)
	{
		//
		return;
	}

	/***********************************************/

	// 상태 변경
	switch (DownBorderState)
	{
	case EDownBorderState::Start:

		break;
	case EDownBorderState::Up:
		TimerOfUp += 0.25f * DeltaTime;
		DownBorderMesh->SetRelativeLocation(FVector(0.0f, 0.0f, Height * FMath::Sin(TimerOfUp * PI)));
		
		if (TimerOfUp >= 0.5f)
		{
			DownBorderState = EDownBorderState::Floating;
			TimerOfFloating = 1.5f;
		}

		break;
	case EDownBorderState::Floating:
		TimerOfFloating += 0.5f * DeltaTime;
		DownBorderMesh->SetRelativeLocation(FVector(0.0f, 0.0f, Height + 64.0f * FMath::Cos(TimerOfFloating * PI)));

		break;
	case EDownBorderState::Down:
		DownBorderMesh->SetRelativeLocation(FVector(0.0f, 0.0f, Height * TimerOfDown));
		TimerOfDown -= 0.5f * DeltaTime;
		

		if (TimerOfDown <= 0.0f)
		{
			if (Occupancy < 100.0f)
				DownBorderState = EDownBorderState::Start;
			else
			{
				DownBorderState = EDownBorderState::End;
				PrimaryActorTick.bCanEverTick = false;
			}

			DownBorderMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		}

		break;
	case EDownBorderState::End:

		break;
	default:

		break;
	}
}
/*** AOccupationPanel : End ***/