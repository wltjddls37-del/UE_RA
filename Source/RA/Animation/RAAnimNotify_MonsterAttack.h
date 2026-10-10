// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RAAnimNotify_MonsterAttack.generated.h"

// 몬스터 공격 몽타주의 타격 시점에 배치, 몬스터의 현재 공격 대상(넥서스 등)에게 데미지
UCLASS(meta = (DisplayName = "RA Monster Attack"))
class RA_API URAAnimNotify_MonsterAttack : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
