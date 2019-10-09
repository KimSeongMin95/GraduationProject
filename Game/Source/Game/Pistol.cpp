// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

// Sets default values
APistol::APistol()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PistolMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Pistol");
	RootComponent = PistolMesh;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (skeletalMeshAsset.Succeeded())
	{
		PistolMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
		PistolMesh->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90도 돌아가 있어서 -90을 해줘야 정방향이 됩니다.
	}

	ArrowComp = CreateDefaultSubobject<UArrowComponent>("ProjectileSpawnPoint");
	ArrowComp->AttachTo(PistolMesh);
	ArrowComp->SetRelativeLocation(FVector(0.0f, 52.59f, 11.18f));
	ArrowComp->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	Super::BeginPlay();
	
	WriteToLogDelegate.BindUObject(this, &APistol::WriteLog);
	WriteToLogDelegate.Execute(TEXT("delegate"));

	
}

// Called every frame
void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APistol::WriteLog(FString string)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, string);

	}
}
//void APistol::PullTrigger_Implementation()
//{
//
//
//}