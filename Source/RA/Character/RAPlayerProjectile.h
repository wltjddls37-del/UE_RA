// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RAPlayerProjectile.generated.h"

//전방선언
//헤더에서는 포인터로만 쓰니까 include 대신 "이런 클래스가 있다" 고만 알려줌
//실제 include는 cpp에서 함 -> 컴파일이 빨라짐
class USphereComponent;
class UStaticMeshComponent;            
class UProjectileMovementComponent;    

//플레이어가 쏘는 총알, 몬스터에 닿으면 데미지를 주고 사라진다.
UCLASS()
class RA_API ARAPlayerProjectile : public AActor
{
	GENERATED_BODY()

public:
	//생성자: 컴포넌트를 여기서 만든다.
	ARAPlayerProjectile();

	//발사한 플레이어가 자기 공격력을 넘겨준다
	//내용이 한줄이라 헤더 안에 바로 작성 (인라인 함수) -> cpp에 안써도됨
	void SetDamage(float InDamage) { Damage = InDamage; }

protected:
	//게임 시작(스폰)시 1번 호출
	virtual void BeginPlay() override;

private:
	//총알이 무언가와 겹쳤을때(Overlap) 호출되는 함수
	//델리게이트(OnComponentBeginOverlap)에 연결하려면 반드시 UFUNCTION()
	//매개변수 6개가 엔진이 정한 모양과 정확히 같아야 함.
	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,   
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 바닥/벽(WorldStatic)에 막혀서 ProjectileMovement 가 멈췄을 때 호출
	// OnProjectileStop 델리게이트 모양: (const FHitResult&) 1개
	UFUNCTION()
	void HandleStop(const FHitResult& ImpactResult);

protected:
	// 충돌 담당 (루트). 이 구가 몬스터와 겹치는지 검사
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> CollisionComponent;              

	// 겉모습. BP 에서 Sphere 같은 메시를 지정
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;               

	// 이동 담당. 지정한 속도로 앞으로 날아가게 해줌
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> MovementComponent;    

	//아무것도 안 맞으면 이 시간(초) 뒤 자동으로 사라짐
	// EditAnywhere -> BP 에서 값 변경 가능
	UPROPERTY(EditAnywhere, Category = "RA")
	float LifeTime = 3.f;                                         

private:
	// SetDamage 로 받아둔 데미지
	float Damage = 0.f;                                           
};