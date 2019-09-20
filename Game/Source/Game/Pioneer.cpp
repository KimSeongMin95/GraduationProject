// Fill out your copyright notice in the Description page of Project Settings.


#include "Pioneer.h"
#include "Engine/Classes/Components/SphereComponent.h" // for SetStaticMesh
#include "UObject/ConstructorHelpers.h"

// 생성시 +1하는 고유값입니다.
int APioneer::ID = 0;

// Sets default values
APioneer::APioneer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ID++;

	TempStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TempStaticMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		TempStaticMesh->SetStaticMesh(SphereVisualAsset.Object);
		TempStaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, (float)ID  * 10.0f));
		TempStaticMesh->SetWorldScale3D(FVector(0.8f));
	}
	TempStaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APioneer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APioneer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APioneer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

