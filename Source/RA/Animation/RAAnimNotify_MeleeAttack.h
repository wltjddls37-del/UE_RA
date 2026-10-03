// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RAAnimNotify_MeleeAttack.generated.h"

// 노티파이 시점에 소켓 + Offset 위치에서 Sphere 충돌, 감지된 몬스터에게 공격력만큼 데미지
UCLASS()
class RA_API URAAnimNotify_MeleeAttack : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	// 충돌 기준 소켓, None 이면 메시 원점
	UPROPERTY(EditAnywhere, Category = "RA")
	FName SocketName;

	// 소켓 기준(소켓 로컬 좌표) 충돌 중심 Offset
	UPROPERTY(EditAnywhere, Category = "RA")
	FVector SocketOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "RA", meta = (ClampMin = "0.0"))
	float AttackRadius = 50.f;
};
