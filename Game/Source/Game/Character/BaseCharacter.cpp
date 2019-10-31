// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Controller/BaseAIController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

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

/*** CharacterMovement : Start ***/
void ABaseCharacter::InitCharacterMovement()
{
	// Don't rotate character to camera direction
	// 컨트롤러가 회전할 떄 각 축을 회전시키지 않습니다. 카메라에만 영향을 줍니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터 이동&회전 관련 설정을 합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향에 캐릭터 메시가 따라 회전합니다.
	//GetCharacterMovement()->bOrientRotationToMovement = false; // 이동 방향에 캐릭터 메시가 따라 회전하지 않습니다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // 캐릭터를 이동시키기 전에 이동 방향과 현재 캐릭터의 방향이 다르면 캐릭터를 이동 방향으로 초당 360도의 회전 속도로 회전시킨다음 이동시킵니다.
	GetCharacterMovement()->bConstrainToPlane = true; // 캐릭터의 이동을 평면으로 제한합니다.
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // 시작할 때 캐릭터의 위치가 평면을 벗어난 상태라면 가까운 평면으로 붙여서 시작되도록 합니다. 여기서 평면이란 내비게이션 메시를 의미합니다.
	//GetCharacterMovement()->JumpZVelocity = 600.0f;
	//GetCharacterMovement()->AirControl = 0.2f;
}

void ABaseCharacter::LookAtTheLocation(FVector Location)
{
	if (!RootComponent)
		return;

	// 현재 rootComponent 위치
	FVector rootCompLocation = RootComponent->GetComponentLocation();

	// 방향을 구합니다.
	FVector direction = FVector(
		Location.X - rootCompLocation.X,
		Location.Y - rootCompLocation.Y,
		Location.Z - rootCompLocation.Z);

	// 벡터를 정규화합니다.
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

	// 180도 이상 차이가 나면 가까운 쪽으로 회전하도록 조정합니다.
	if (DifferenceYaw > 180.0f)
		sign = -1.0f;

	// 흔들림 방지용
	bool under = false; // CurrentRotation.Yaw가 TargetRotation.Yaw보다 작은 상태
	bool upper = false; // CurrentRotation.Yaw가 TargetRotation.Yaw보다 큰 상태

	// 회전값을 점진적으로 조정합니다.
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

	// 작았었는데 커졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	if (upper && CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}
	// 컸었는데 작아졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}

	// 변경된 각도로 다시 설정합니다.
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