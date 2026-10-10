// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h" // 부모 클래스 UAnimNotify
#include "RAAnimNotify_FireProjectile.generated.h" // 항상 include "generated.h"를 마지막에 포함시켜야 합니다.

// 전방 선언 (실제 include는 cpp에서)
class ARAPlayerProjectile; // 발사할 투사체 클래스

//플레이어 공격 몽타주에 배치
//노티파이 시점에 총알(RAPlayerProjectile)을 카메라가 바라보는 방향으로 발사하는 기능을 구현
//DisplayName -> 몽타주에서 노티파이 추가할때 목록에 보이는 이름
UCLASS(meta = (DisplayName = "Fire Projectile"))
class RA_API URAAnimNotify_FireProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	//애니메이션이 이 노티파이 위치를 지나갈 때 엔진이 호출하는 함수
	// 부모(UAnimNotify)에 있는 virtual 함수라서 override
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	//몽타주에서 노티파이를 클릭하면 디테일 패널에 보이는 값 들
	//EditAnywhere -> 에디터에서 수정 가능

	//발사할 총알 블루프린트(BP_RAPlayerProjectile을 지정)
	//TSubclassOf = T 또는 T의 자식 클래스만 지정 가능한 클래스 타입
	UPROPERTY(EditAnywhere, Category = "RA")
	TSubclassOf<ARAPlayerProjectile> ProjectileClass;

	//총알이 나올 소켓이름 (예 : 총구 ,오른손)
	//비워두면(None) 캐릭터 앞쪽에서 발사
	UPROPERTY(EditAnywhere, Category = "RA")
	FName SocketName;

	// 소켓이 없을때 캐릭터 기준 발사 위치 (x = 앞, y = 좌우, z = 위아래)
	UPROPERTY(EditAnywhere, Category = "RA")
	FVector SpawnOffset = FVector(100.f, 0.f, 50.f);
};