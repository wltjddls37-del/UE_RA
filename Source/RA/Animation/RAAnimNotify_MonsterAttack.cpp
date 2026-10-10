// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/RAAnimNotify_MonsterAttack.h"
#include "Components/SkeletalMeshComponent.h"
#include "Monster/RAMonster.h"

void URAAnimNotify_MonsterAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// 애니메이션 에디터 프리뷰 등 몬스터가 아닌 경우는 무시
	if (ARAMonster* Monster = MeshComp ? Cast<ARAMonster>(MeshComp->GetOwner()) : nullptr)
	{
		Monster->ApplyAttackDamage();
	}
}
