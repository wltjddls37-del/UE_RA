// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RACharacter.h"
#include "RAMonster.generated.h"

class UWidgetComponent;
class UAnimMontage;
class UAnimSequenceBase;
class ARAProjectile;

UCLASS()
class RA_API ARAMonster : public ARACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARAMonster();

	virtual void Die() override;

	// 공격 시작, 공격 중이면 무시
	// AttackMontage 가 있으면 재생 (데미지는 RAAnimNotify_MonsterAttack 시점), 없으면 즉시 데미지 후 AttackInterval 대기
	void StartAttack(AActor* Target);

	// RAAnimNotify_MonsterAttack 에서 호출, 공격 대상이 공격 거리 안에 있으면 공격력만큼 데미지
	void ApplyAttackDamage();

	bool IsAttacking() const { return bIsAttacking; }

	float GetAttackDistance() const { return AttackDistance; }

	// 서로의 충돌 반경을 뺀 표면 사이 거리 (넥서스처럼 큰 액터도 가장자리 기준)
	float GetDistanceToTarget(const AActor* Target) const;

	bool IsInAttackRange(const AActor* Target) const;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void FinishAttack();

	void FireProjectile(AActor* Target, float Damage);

protected:
	// 머리 위 체력바, Widget Class 는 BP 에서 WBP_MonsterHealthBarWidget 지정
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> HealthBarWidget;

	// 공격 거리 (표면 기준), 근접 / 원거리 몬스터 BP 에서 각각 설정
	UPROPERTY(EditAnywhere, Category = "RA|Attack", meta = (ClampMin = "0.0"))
	float AttackDistance = 150.f;

	// 공격 몽타주, RAAnimNotify_MonsterAttack 노티파이를 타격 시점에 배치
	UPROPERTY(EditAnywhere, Category = "RA|Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	// AttackMontage 가 없을 때 공격 간격 (초)
	UPROPERTY(EditAnywhere, Category = "RA|Attack", meta = (ClampMin = "0.1"))
	float AttackInterval = 1.5f;

	// 원거리 몬스터만 지정, 지정하면 공격 시 투사체 발사 (비우면 근접 공격)
	UPROPERTY(EditAnywhere, Category = "RA|Attack")
	TSubclassOf<ARAProjectile> ProjectileClass;

	// 투사체 발사 위치 소켓, 없으면 몬스터 앞쪽
	UPROPERTY(EditAnywhere, Category = "RA|Attack")
	FName MuzzleSocketName;

	// 사망 애니메이션 (Death_A 등), 마지막 프레임에서 멈춘다
	UPROPERTY(EditAnywhere, Category = "RA|Death")
	TObjectPtr<UAnimSequenceBase> DeathAnimation;

	// 사망 후 제거까지 시간 (초)
	UPROPERTY(EditAnywhere, Category = "RA|Death", meta = (ClampMin = "0.1"))
	float DeathLifeSpan = 3.f;

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> AttackTarget;

	bool bIsAttacking = false;

	FTimerHandle AttackTimerHandle;
};
