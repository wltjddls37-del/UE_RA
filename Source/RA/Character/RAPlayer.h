// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RACharacter.h"
#include "RAPlayer.generated.h"

class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class UInputAction;
class UParticleSystem;
struct  FInputActionValue;

UCLASS()
class RA_API ARAPlayer : public ARACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARAPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void Move(const FInputActionValue& InValue);
	void Look(const FInputActionValue& InValue);
	void Attack();

	// 플레이어 공격으로 몬스터를 처치했을 때 호출, KillsPerPowerUp 마다 강화
	void AddKill();

	int32 GetKillCount() const { return KillCount; }

	// ── 조준 (크로스헤어) ──
	// 화면 중앙(카메라 정면)으로 라인트레이스를 쏴서 크로스헤어가 가리키는 곳을 찾는다
	// 무언가 맞으면 true, 안 맞으면 false
	// 어느 쪽이든 OutHit.ImpactPoint 에는 "총알이 향할 목표 지점" 이 들어있다 (안 맞으면 사거리 끝)
	bool TraceAim(FHitResult& OutHit) const;

private:
	void PlayNextAttackMontage();
	void ResetAttack();

	UFUNCTION()
	void HandleMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	// 공격력 / 최대 체력 강화 + 이펙트
	void PowerUp();

protected:
	UPROPERTY(EditAnywhere, Category = "RA")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "RA")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "RA")
	TObjectPtr<UInputAction> LookAction;
		
	UPROPERTY(EditAnywhere, Category = "RA")
	TObjectPtr<UInputAction> AttackAction;

	// 공격 입력마다 Index 순서대로 재생
	UPROPERTY(EditAnywhere, Category = "RA")
	TArray<TObjectPtr<UAnimMontage>> AttackMontageArray;

	// 몇 마리 처치마다 강화할지
	UPROPERTY(EditAnywhere, Category = "RA|PowerUp", meta = (ClampMin = "1"))
	int32 KillsPerPowerUp = 10;

	// 강화 1회당 공격력 증가량
	UPROPERTY(EditAnywhere, Category = "RA|PowerUp")
	float PowerUpAttackBonus = 10.f;

	// 강화 1회당 최대 체력 증가량 (현재 체력도 같이 증가)
	UPROPERTY(EditAnywhere, Category = "RA|PowerUp")
	float PowerUpHealthBonus = 50.f;

	// 강화 이펙트 (Cascade 파티클), 플레이어에 붙어서 재생
	UPROPERTY(EditAnywhere, Category = "RA|PowerUp")
	TObjectPtr<UParticleSystem> PowerUpEffect;

	// 조준 라인트레이스 최대 거리 (사거리)
	UPROPERTY(EditAnywhere, Category = "RA|Aim")
	float AimTraceDistance = 10000.f;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent>	SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

private:
	// 재생 중인 공격 몽타주, 없으면 nullptr
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAttackMontage;

	// 다음에 재생할 AttackMontageArray Index
	int32 AttackIndex = 0;

	// 재생 중에 들어온 공격 입력, 현재 몽타주가 끝나면 다음 몽타주 재생
	bool bAttackInputQueued = false;

	// 처치한 몬스터 수
	int32 KillCount = 0;
};
