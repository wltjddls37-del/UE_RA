// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/RAMonster.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Component/RAStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Monster/RAProjectile.h"
#include "TimerManager.h"
#include "UI/RAMonsterHealthBarWidget.h"

// Sets default values
ARAMonster::ARAMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	// 스포너에서 생성되었을 때도 AIController 가 빙의하도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 이동 방향을 바라보며 걷기
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

	// 몬스터끼리 서로 비켜 가도록 RVO 회피 사용
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 200.f;

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(100.f, 10.f));
	HealthBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARAMonster::BeginPlay()
{
	// 위젯은 Super::BeginPlay 에서 WidgetComponent 가 생성
	Super::BeginPlay();

	if (URAMonsterHealthBarWidget* Widget = Cast<URAMonsterHealthBarWidget>(HealthBarWidget->GetUserWidgetObject()))
	{
		StatComponent->OnUpdateStat.AddDynamic(Widget, &URAMonsterHealthBarWidget::UpdateStat);

		// StatComponent 는 이미 초기화되었으므로 현재 체력 반영
		Widget->UpdateStat(ERAStatType::Health, StatComponent->GetValue(ERAStatType::Health), StatComponent->GetBaseValue(ERAStatType::Health));
	}

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ARAMonster::HandleMontageEnded);
	}
}

void ARAMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Die 를 거치지 않고 제거된 경우(낙사 등)에도 사망으로 알려 웨이브가 끝나지 않는 문제를 막는다
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		Die();
	}

	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &ARAMonster::HandleMontageEnded);
	}

	// WidgetComponent 가 위젯을 해제하기 전(Super::EndPlay)에 바인딩 해제
	if (URAMonsterHealthBarWidget* Widget = Cast<URAMonsterHealthBarWidget>(HealthBarWidget->GetUserWidgetObject()))
	{
		StatComponent->OnUpdateStat.RemoveDynamic(Widget, &URAMonsterHealthBarWidget::UpdateStat);
	}

	Super::EndPlay(EndPlayReason);
}

void ARAMonster::Die()
{
	if (IsDead())
	{
		return;
	}

	Super::Die();

	// 공격 중단
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	bIsAttacking = false;
	AttackTarget = nullptr;

	// 죽은 몬스터가 계속 이동 / 공격하지 않도록 AI 정지
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();

		if (UBrainComponent* BrainComponent = AIController->GetBrainComponent())
		{
			BrainComponent->StopLogic(TEXT("Dead"));
		}
	}

	// 액터가 이미 제거되는 중이면(EndPlay) 연출 생략
	if (IsActorBeingDestroyed())
	{
		return;
	}

	HealthBarWidget->SetVisibility(false);

	// 몽타주와 달리 PlayAnimation 은 끝난 뒤 마지막 프레임을 유지한다
	if (DeathAnimation)
	{
		GetMesh()->PlayAnimation(DeathAnimation, false);
	}

	SetLifeSpan(DeathLifeSpan);
}

void ARAMonster::StartAttack(AActor* Target)
{
	if (IsDead() || bIsAttacking || Target == nullptr)
	{
		return;
	}

	AttackTarget = Target;
	bIsAttacking = true;

	// 몽타주가 재생되면 데미지는 노티파이에서, 종료는 HandleMontageEnded 에서 처리
	if (AttackMontage && PlayAnimMontage(AttackMontage) > 0.f)
	{
		return;
	}

	// 몽타주가 없으면 즉시 데미지, AttackInterval 뒤 다음 공격 가능
	ApplyAttackDamage();
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ARAMonster::FinishAttack, AttackInterval, false);
}

void ARAMonster::ApplyAttackDamage()
{
	AActor* Target = AttackTarget.Get();
	if (IsDead() || Target == nullptr)
	{
		return;
	}

	// 공격 도중 대상이 멀어졌으면 빗나감
	if (IsInAttackRange(Target) == false)
	{
		return;
	}

	const float Damage = StatComponent->GetValue(ERAStatType::AttackPower);

	// 원거리 몬스터는 투사체가 맞았을 때 데미지
	if (ProjectileClass)
	{
		FireProjectile(Target, Damage);
		return;
	}

	UGameplayStatics::ApplyDamage(Target, Damage, GetController(), this, nullptr);
}

void ARAMonster::FireProjectile(AActor* Target, float Damage)
{
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * (GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.f);

	if (MuzzleSocketName.IsNone() == false && GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		SpawnLocation = GetMesh()->GetSocketLocation(MuzzleSocketName);
	}

	// 넥서스처럼 원점이 바닥에 있는 액터도 몸통을 향해 쏘도록 바운드 중심을 조준
	FVector TargetOrigin;
	FVector TargetExtent;
	Target->GetActorBounds(true, TargetOrigin, TargetExtent);

	const FRotator SpawnRotation = (TargetOrigin - SpawnLocation).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ARAProjectile* Projectile = GetWorld()->SpawnActor<ARAProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams))
	{
		Projectile->SetDamage(Damage);
	}
}

float ARAMonster::GetDistanceToTarget(const AActor* Target) const
{
	if (Target == nullptr)
	{
		return MAX_flt;
	}

	float TargetRadius = 0.f;
	float TargetHalfHeight = 0.f;
	Target->GetSimpleCollisionCylinder(TargetRadius, TargetHalfHeight);

	const float CenterDistance = FVector::Dist2D(GetActorLocation(), Target->GetActorLocation());
	const float MyRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();

	return FMath::Max(0.f, CenterDistance - TargetRadius - MyRadius);
}

bool ARAMonster::IsInAttackRange(const AActor* Target) const
{
	return GetDistanceToTarget(Target) <= AttackDistance;
}

void ARAMonster::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage && Montage == AttackMontage)
	{
		FinishAttack();
	}
}

void ARAMonster::FinishAttack()
{
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	bIsAttacking = false;
}
