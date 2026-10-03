// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/RAMonster.h"

// Sets default values
ARAMonster::ARAMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	// 스포너에서 생성되었을 때도 AIController 가 빙의하도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ARAMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Die 를 거치지 않고 제거된 경우(낙사 등)에도 사망으로 알려 웨이브가 끝나지 않는 문제를 막는다
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		Die();
	}

	Super::EndPlay(EndPlayReason);
}
