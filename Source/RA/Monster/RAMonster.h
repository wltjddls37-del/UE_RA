// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RACharacter.h"
#include "RAMonster.generated.h"

UCLASS()
class RA_API ARAMonster : public ARACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARAMonster();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
