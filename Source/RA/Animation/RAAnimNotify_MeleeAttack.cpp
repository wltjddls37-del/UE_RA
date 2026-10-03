// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/RAAnimNotify_MeleeAttack.h"
#include "Character/RACharacter.h"
#include "Component/RAStatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Monster/RAMonster.h"

void URAAnimNotify_MeleeAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	UWorld* World = MeshComp->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const FVector Center = MeshComp->GetSocketTransform(SocketName).TransformPosition(SocketOffset);

#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(World, Center, AttackRadius, 16, FColor::Red, false, 1.f);
#endif

	// 애니메이션 에디터 프리뷰 등 RACharacter 가 아닌 경우는 디버그 표시만
	ARACharacter* Attacker = Cast<ARACharacter>(MeshComp->GetOwner());
	if (Attacker == nullptr)
	{
		return;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RAMeleeAttack), false, Attacker);

	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByObjectType(OverlapResults, Center, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(AttackRadius), QueryParams);

	// 한 액터에 여러 컴포넌트가 겹칠 수 있으므로 몬스터 단위로 중복 제거
	TSet<ARAMonster*> HitMonsterSet;
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (ARAMonster* Monster = Cast<ARAMonster>(OverlapResult.GetActor()))
		{
			HitMonsterSet.Add(Monster);
		}
	}

	const float Damage = Attacker->GetStatComponent()->GetValue(ERAStatType::AttackPower);
	for (ARAMonster* Monster : HitMonsterSet)
	{
		UGameplayStatics::ApplyDamage(Monster, Damage, Attacker->GetController(), Attacker, nullptr);
	}
}
