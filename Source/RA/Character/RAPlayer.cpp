#include "RAPlayer.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimMontage.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/RAStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Engine/World.h" // LineTraceSingleByObjectType

// Sets default values
ARAPlayer::ARAPlayer()
{ 	
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);


	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 500.f;
	SpringArmComponent->bUsePawnControlRotation = true;
	// 카메라를 오른쪽 어깨 위로 이동 (Y = 오른쪽, Z = 위)
	// → 화면 중앙(크로스헤어)이 캐릭터 몸에 가려지지 않는다
	SpringArmComponent->SocketOffset = FVector(0.f, 60.f, 60.f);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ARAPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageBlendingOut.AddDynamic(this, &ARAPlayer::HandleMontageBlendingOut);
	}
}

void ARAPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &ARAPlayer::HandleMontageBlendingOut);
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ARAPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARAPlayer::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ARAPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
		
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{	
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::Attack);
	}
}

void ARAPlayer::Move(const FInputActionValue& InValue)
{
	FVector2D MovementVector = InValue.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotator(0, Rotation.Yaw, 0);

		const FVector ForwardVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardVector, MovementVector.Y);
		AddMovementInput(RightVector, MovementVector.X);

	}
}

void ARAPlayer::Look(const FInputActionValue& InValue)
{
	FVector2D LookDirection = InValue.Get<FVector2D>();

	AddControllerYawInput(LookDirection.X);
	AddControllerPitchInput(LookDirection.Y);
}

void ARAPlayer::Attack()
{
	if (AttackMontageArray.IsEmpty())
	{
		return;
	}

	// 재생 중인 몽타주는 끊지 않고, 끝난 뒤 다음 몽타주를 재생하도록 입력만 저장
	if (CurrentAttackMontage)
	{
		bAttackInputQueued = true;
		return;
	}

	PlayNextAttackMontage();
}

void ARAPlayer::PlayNextAttackMontage()
{
	bAttackInputQueued = false;

	UAnimMontage* NextMontage = AttackMontageArray[AttackIndex];
	AttackIndex = (AttackIndex + 1) % AttackMontageArray.Num();

	// 재생 중 이전 몽타주의 BlendingOut 콜백이 다시 들어와도 무시되도록 재생 전에 갱신
	CurrentAttackMontage = NextMontage;

	if (NextMontage == nullptr || PlayAnimMontage(NextMontage) <= 0.f)
	{
		ResetAttack();
	}
}

void ARAPlayer::ResetAttack()
{
	CurrentAttackMontage = nullptr;
	AttackIndex = 0;
	bAttackInputQueued = false;
}

void ARAPlayer::HandleMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == nullptr || Montage != CurrentAttackMontage)
	{
		return;
	}

	// 다른 몽타주에 의해 끊겼거나 추가 입력이 없으면 콤보 종료, 다음 공격은 처음부터
	if (bInterrupted || bAttackInputQueued == false)
	{
		ResetAttack();
		return;
	}

	PlayNextAttackMontage();
}

void ARAPlayer::AddKill()
{
	++KillCount;

	if (KillCount % KillsPerPowerUp == 0)
	{
		PowerUp();
	}
}

bool ARAPlayer::TraceAim(FHitResult& OutHit) const
{
	UWorld* World = GetWorld();
	if (World == nullptr || CameraComponent == nullptr)
	{
		return false;
	}

	// 카메라 위치 + 카메라가 보는 방향 = 화면 정중앙으로 나가는 선
	const FVector CameraLocation = CameraComponent->GetComponentLocation();
	const FVector CameraForward = CameraComponent->GetForwardVector();

	// ★ 시작점을 캐릭터 옆까지 앞으로 당긴다
	//   카메라에서 바로 쏘면 카메라와 캐릭터 "사이" 에 있는 몬스터(캐릭터 뒤쪽)가 맞을 수 있음
	//   DotProduct = 카메라→캐릭터 벡터를 카메라 정면 방향으로 투영한 길이 (= 앞으로 얼마나 떨어져 있나)
	const float DistanceToPlayer = FVector::DotProduct(GetActorLocation() - CameraLocation, CameraForward);
	const FVector Start = CameraLocation + CameraForward * FMath::Max(DistanceToPlayer, 0.f);
	const FVector End = CameraLocation + CameraForward * AimTraceDistance;

	// 어떤 종류(ObjectType)의 물체를 맞힐지
	//  - WorldStatic : 바닥, 벽
	//  - Pawn        : 몬스터 캡슐
	//  (WorldDynamic 은 뺌 → 날아가는 총알에 조준이 걸리는 것 방지)
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	// 자기 자신(플레이어)은 무시
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RAAimTrace), false, this);

	const bool bHit = World->LineTraceSingleByObjectType(OutHit, Start, End, ObjectParams, QueryParams);

	// 아무것도 안 맞았으면 사거리 끝을 목표 지점으로
	if (bHit == false)
	{
		OutHit.ImpactPoint = End;
		OutHit.Location = End;
	}

	return bHit;
}

void ARAPlayer::PowerUp()
{
	StatComponent->AddBaseValue(ERAStatType::AttackPower, PowerUpAttackBonus);
	StatComponent->AddBaseValue(ERAStatType::Health, PowerUpHealthBonus);

	if (PowerUpEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(PowerUpEffect, GetRootComponent());
	}
}
