// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Controller/BaseAIController.h"
/*** ���� ������ ��� ���� ���� : End ***/

/*** Basic Function : Start ***/
ABaseCharacter::ABaseCharacter() // Sets default values
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bRotateTargetRotation = false;
	TargetRotation = FRotator::ZeroRotator;

	Health = 100.0f;
	bDead = false;

	InitCharacterMovement();
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

/*** CharacterMovement : Start ***/
void ABaseCharacter::InitCharacterMovement()
{
	// Don't rotate character to camera direction
	// ��Ʈ�ѷ��� ȸ���� �� �� ���� ȸ����Ű�� �ʽ��ϴ�. ī�޶󿡸� ������ �ݴϴ�.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ĳ���� �̵�&ȸ�� ���� ������ �մϴ�.
	GetCharacterMovement()->bOrientRotationToMovement = true; // �̵� ���⿡ ĳ���� �޽ð� ���� ȸ���մϴ�.
	//GetCharacterMovement()->bOrientRotationToMovement = false; // �̵� ���⿡ ĳ���� �޽ð� ���� ȸ������ �ʽ��ϴ�.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ĳ���͸� �̵���Ű�� ���� �̵� ����� ���� ĳ������ ������ �ٸ��� ĳ���͸� �̵� �������� �ʴ� 360���� ȸ�� �ӵ��� ȸ����Ų���� �̵���ŵ�ϴ�.
	GetCharacterMovement()->bConstrainToPlane = true; // ĳ������ �̵��� ������� �����մϴ�.
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // ������ �� ĳ������ ��ġ�� ����� ��� ���¶�� ����� ������� �ٿ��� ���۵ǵ��� �մϴ�. ���⼭ ����̶� ������̼� �޽ø� �ǹ��մϴ�.
	//GetCharacterMovement()->JumpZVelocity = 600.0f;
	//GetCharacterMovement()->AirControl = 0.2f;
}

void ABaseCharacter::LookAtTheLocation(FVector Location)
{
	if (!RootComponent)
		return;

	// ���� rootComponent ��ġ
	FVector rootCompLocation = RootComponent->GetComponentLocation();

	// ������ ���մϴ�.
	FVector direction = FVector(
		Location.X - rootCompLocation.X,
		Location.Y - rootCompLocation.Y,
		Location.Z - rootCompLocation.Z);

	// ���͸� ����ȭ�մϴ�.
	direction.Normalize();

	TargetRotation = FRotator(
		RootComponent->GetComponentRotation().Pitch,
		direction.Rotation().Yaw,
		RootComponent->GetComponentRotation().Roll);

	bRotateTargetRotation = true;
}

void ABaseCharacter::RotateTargetRotation(float DeltaTime)
{
	if (!RootComponent || !GetCharacterMovement())
		return;

	FRotator CurrentRotation = RootComponent->GetComponentRotation(); // Normalized

	FRotator DeltaRot = GetCharacterMovement()->GetDeltaRotation(DeltaTime);

	float sign = 1.0f;
	float DifferenceYaw = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	// 180�� �̻� ���̰� ���� ����� ������ ȸ���ϵ��� �����մϴ�.
	if (DifferenceYaw > 180.0f)
		sign = -1.0f;

	// ��鸲 ������
	bool under = false; // CurrentRotation.Yaw�� TargetRotation.Yaw���� ���� ����
	bool upper = false; // CurrentRotation.Yaw�� TargetRotation.Yaw���� ū ����

	// ȸ������ ���������� �����մϴ�.
	if (CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation.Yaw += DeltaRot.Yaw * sign;
		under = true;
	}
	else
	{
		CurrentRotation.Yaw -= DeltaRot.Yaw * sign;
		upper = true;
	}

	// �۾Ҿ��µ� Ŀ���ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	if (upper && CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}
	// �Ǿ��µ� �۾����ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}

	// ����� ������ �ٽ� �����մϴ�.
	RootComponent->SetWorldRotation(CurrentRotation);
}
/*** CharacterMovement : End ***/

/*** State : Start ***/
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
/*** State : End ***/