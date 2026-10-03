// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RACharacter.h"
#include "RAMonster.generated.h"

class UWidgetComponent;

UCLASS()
class RA_API ARAMonster : public ARACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARAMonster();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// 머리 위 체력바, Widget Class 는 BP 에서 WBP_MonsterHealthBarWidget 지정
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> HealthBarWidget;
};
