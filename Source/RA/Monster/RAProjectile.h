// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RAProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

// 원거리 몬스터 투사체, 처음 부딪힌 대상(넥서스 등)에게 데미지를 주고 사라진다
// Pawn 은 통과하므로 다른 몬스터나 플레이어에게 막히지 않는다
UCLASS()
class RA_API ARAProjectile : public AActor
{
	GENERATED_BODY()

public:
	ARAProjectile();

	void SetDamage(float InDamage) { Damage = InDamage; }

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> CollisionComponent;

	// 투사체 모양, BP 에서 Static Mesh 지정 (예: 구체)
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	// 투사체 이펙트, BP 에서 Template 지정 (선택)
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystemComponent> TrailComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> MovementComponent;

	// 아무것도 맞히지 못했을 때 자동 제거 시간 (초)
	UPROPERTY(EditAnywhere, Category = "RA", meta = (ClampMin = "0.1"))
	float LifeTime = 5.f;

private:
	float Damage = 0.f;
};
