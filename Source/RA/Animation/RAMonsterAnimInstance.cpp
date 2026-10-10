// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/RAMonsterAnimInstance.h"
#include "GameFramework/Pawn.h"

void URAMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const APawn* Pawn = TryGetPawnOwner())
	{
		Speed = Pawn->GetVelocity().Size2D();
	}
}
