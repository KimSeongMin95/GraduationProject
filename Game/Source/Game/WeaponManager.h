// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** �𸮾��� ��� ���� : Start ***/
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h" // TActorIterator<>

/*** �𸮾��� ��� ���� : End ***/


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponManager.generated.h"

UCLASS()
class GAME_API AWeaponManager : public AActor
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:	
	AWeaponManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AWeaponManager : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "AWeaponManager")
		/** RootComponent */
		class USceneComponent* SceneComp = nullptr;

/*** AWeaponManager : End ***/
};
