#include "BTTask_MonsterTrace.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MonsterTrace::UBTTask_MonsterTrace()
{
	NodeName = TEXT("MonsterTrace");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MonsterTrace::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComp)
		return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("Target")));

	if (!Target)
		return EBTNodeResult::Failed;

	// 타겟을 따라간다.
	AIController->MoveToActor(Target);

	return EBTNodeResult::InProgress;
}

void UBTTask_MonsterTrace::TickTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* Pawn = AIController->GetPawn();
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("Target")));

	if (!Pawn || !Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	float AttackDistance = BlackboardComp->GetValueAsFloat(TEXT("AttackDistance"));

	// 블랙보드에 값이 없으면 기본값 사용
	if (AttackDistance <= 0.f)
		AttackDistance = 150.f;

	float Distance = FVector::Dist2D(Pawn->GetActorLocation(),
		Target->GetActorLocation());

	// 공격 거리 안에 들어오면 Failed -> Selector가 Attack 실행
	if (Distance <= AttackDistance)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

void UBTTask_MonsterTrace::OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (AAIController* AIController = OwnerComp.GetAIOwner())
		AIController->StopMovement();
}