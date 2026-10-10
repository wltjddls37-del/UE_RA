// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Subsystem/RAWaveSubSystem.h"
#include "Nexus/RANexus.h"

// 몬스터 BT Task 공용 함수
namespace RAMonsterAI
{
	// 블랙보드 Target 이 비어 있으면 넥서스를 Target 으로 지정한다
	// Target 이 있거나 지정에 성공하면 true, 넥서스가 없거나 파괴되었으면 false
	inline bool EnsureNexusTarget(UBehaviorTreeComponent& OwnerComp)
	{
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (BlackboardComp == nullptr)
		{
			return false;
		}

		if (AActor* CurrentTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("Target"))))
		{
			// 파괴된 넥서스는 타겟에서 제외
			const ARANexus* CurrentNexus = Cast<ARANexus>(CurrentTarget);
			if (CurrentNexus == nullptr || CurrentNexus->IsDestroyed() == false)
			{
				return true;
			}

			BlackboardComp->ClearValue(TEXT("Target"));
			return false;
		}

		UWorld* World = OwnerComp.GetWorld();
		URAWaveSubSystem* WaveSubSystem = World ? World->GetSubsystem<URAWaveSubSystem>() : nullptr;
		ARANexus* Nexus = WaveSubSystem ? WaveSubSystem->GetNexus() : nullptr;

		if (Nexus == nullptr || Nexus->IsDestroyed())
		{
			return false;
		}

		BlackboardComp->SetValueAsObject(TEXT("Target"), Nexus);
		return true;
	}

	// 타겟이 파괴된 넥서스이면 true
	inline bool IsInvalidTarget(const AActor* Target)
	{
		if (Target == nullptr)
		{
			return true;
		}

		const ARANexus* Nexus = Cast<ARANexus>(Target);
		return Nexus && Nexus->IsDestroyed();
	}
}
