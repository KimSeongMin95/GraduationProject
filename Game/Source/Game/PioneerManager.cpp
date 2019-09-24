// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"
#include "Pioneer.h"
#include "Engine/World.h"

// Sets default values
APioneerManager::APioneerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APioneerManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APioneerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}