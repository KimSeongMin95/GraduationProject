// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "BaseAIController.h"
/*** ���� ������ ��� ���� ���� : End ***/

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
	// �ڽ� Ŭ�������� ������ ��!
}

void ABaseCharacter::PossessAIController()
{
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("!AIController"));
		return;
	}

	// �����ϰ� �ϱ� ���� ���� ���� ��Ʈ�ѷ��� ������ ������ ���Ǹ� �����մϴ�.
	if (GetController())
	{
		GetController()->UnPossess();
	}

	// �׵� AI ��Ʈ�ѷ��� �����մϴ�.
	AIController->Possess(this);
}
/*** AIController : End ***/



/*** ���� �����ϱ� : Start ***/
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
/*** ���� �����ϱ� : End ***/