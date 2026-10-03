// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/RABaseStatRow.h"
#include "RACharacter.generated.h"

class ARACharacter;
class URAStatComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDied, ARACharacter*, Character);

// 플레이어, 몬스터 공용 부모
UCLASS()
class RA_API ARACharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARACharacter();

	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// 체력이 0 이 되면 호출, 자식 클래스에서 override 하여 사망 애니메이션 등을 추가
	virtual void Die();

	bool IsDead() const { return bIsDead; }

	URAStatComponent* GetStatComponent() const { return StatComponent; }

	FOnCharacterDied OnCharacterDied;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URAStatComponent> StatComponent;

private:
	bool bIsDead = false;
};
