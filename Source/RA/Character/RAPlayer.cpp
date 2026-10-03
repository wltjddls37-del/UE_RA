#include "RAPlayer.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimMontage.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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