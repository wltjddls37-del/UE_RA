// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RAMonsterAnimInstance.generated.h"

// 몬스터 AnimBP 의 부모 클래스, 블렌드스페이스에 연결할 이동 속도를 매 프레임 계산한다
UCLASS()
class RA_API URAMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 수평 이동 속도, AnimGraph 에서 블렌드스페이스 입력으로 사용
	UPROPERTY(BlueprintReadOnly, Category = "RA")
	float Speed = 0.f;
};
