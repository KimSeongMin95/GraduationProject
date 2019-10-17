// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "BaseAIController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
ABaseCharacter::ABaseCharacter() // Sets default values
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
/*** Basic Function : End ***/

/*** AIController : Start ***/
void ABaseCharacter::InitAIController()
{
	// 자식 클래스에서 구현할 것!
}

void ABaseCharacter::PossessAIController()
{
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("!AIController"));
		return;
	}

	// 안전하게 하기 위해 현재 폰이 컨트롤러를 가지고 있으면 빙의를 해제합니다.
	if (GetController())
	{
		GetController()->UnPossess();
	}

	// 그뒤 AI 컨트롤러를 빙의합니다.
	AIController->Possess(this);
}
/*** AIController : End ***/



/*** 영상 따라하기 : Start ***/
void ABaseCharacter::Calculatehealth(float Delta)
{
	Health += Delta;
	CalculateDead();
}

void ABaseCharacter::CalculateDead()
{
	if (Health <= 0.0f)
		bDead = true;
	else
		bDead = false;
}

#if WITH_EDITOR
void ABaseCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	bDead = false;
	Health = 100;

	Super::PostEditChangeProperty(PropertyChangedEvent);

	CalculateDead();
}
#endif
/*** 영상 따라하기 : End ***/