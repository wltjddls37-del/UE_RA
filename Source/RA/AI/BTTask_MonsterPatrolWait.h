#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterPatrolWait.generated.h"

struct FPatrolWaitMemory
{
	float ElapsedTime = 0.f;
};

UCLASS()
class RA_API UBTTask_MonsterPatrolWait : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MonsterPatrolWait();

protected:
	UPROPERTY(EditAnywhere, Category = "Wait")
	float mWaitTime = 2.f;

public:
	virtual uint16 GetInstanceMemorySize() const override;

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, float DeltaSeconds) override;
};