#include "BTTask_MonsterPatrolWait.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MonsterPatrolWait::UBTTask_MonsterPatrolWait()
{
	NodeName = TEXT("MonsterPatrolWait");
	bNotifyTick = true;
}

uint16 UBTTask_MonsterPatrolWait::GetInstanceMemorySize() const
{
	return sizeof(FPatrolWaitMemory);
}

EBTNodeResult::Type UBTTask_MonsterPatrolWait::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController || !AIController->GetPawn())
		return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (BlackboardComp && BlackboardComp->GetValueAsObject(TEXT("Target")))
		return EBTNodeResult::Failed;

	FPatrolWaitMemory* Memory = (FPatrolWaitMemory*)NodeMemory;
	Memory->ElapsedTime = 0.f;

	AIController->StopMovement();

	return EBTNodeResult::InProgress;
}

void UBTTask_MonsterPatrolWait::TickTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!BlackboardComp || BlackboardComp->GetValueAsObject(TEXT("Target")))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FPatrolWaitMemory* Memory = (FPatrolWaitMemory*)NodeMemory;
	Memory->ElapsedTime += DeltaSeconds;

	// 대기 끝 -> Failed -> Selector가 Patrol 실행
	if (Memory->ElapsedTime >= mWaitTime)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}