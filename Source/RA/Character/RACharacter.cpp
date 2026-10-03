// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RACharacter.h"
#include "Component/RAStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ARACharacter::ARACharacter()
{
	StatComponent = CreateDefaultSubobject<URAStatComponent>(TEXT("Stat"));
}

void ARACharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatComponent->OnUpdateStat.AddDynamic(this, &ARACharacter::HandleStatChanged);
}

void ARACharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StatComponent->OnUpdateStat.RemoveDynamic(this, &ARACharacter::HandleStatChanged);

	Super::EndPlay(EndPlayReason);
}

float ARACharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return 0.f;
	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage <= 0.f)
	{
		return 0.f;
	}

	// 체력이 0 이 되면 HandleStatChanged 에서 Die 호출
	StatComponent->AddValue(ERAStatType::Health, -ActualDamage);

	return ActualDamage;
}

void ARACharacter::HandleStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue)
{
	if (StatType == ERAStatType::Health && FMath::IsNearlyZero(CurrentValue))
	{
		Die();
	}
}

void ARACharacter::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnCharacterDied.Broadcast(this);
}
