// Fill out your copyright notice in the Description page of Project Settings.


#include "Nexus/RANexus.h"
#include "Components/StaticMeshComponent.h"
#include "Component/RAStatComponent.h"
#include "Subsystem/RAWaveSubSystem.h"

// Sets default values
ARANexus::ARANexus()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);

	StatComponent = CreateDefaultSubobject<URAStatComponent>(TEXT("Stat"));
}

void ARANexus::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatComponent->OnStatChanged.AddDynamic(this, &ARANexus::HandleStatChanged);
}

float ARANexus::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDestroyed)
	{
		return 0.f;
	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage <= 0.f)
	{
		return 0.f;
	}

	// 체력이 0 이 되면 HandleStatChanged 에서 HandleDestroyed 호출
	StatComponent->AddValue(ERAStatType::Health, -ActualDamage);

	return ActualDamage;
}

void ARANexus::HandleStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue)
{
	if (StatType == ERAStatType::Health && FMath::IsNearlyZero(CurrentValue))
	{
		HandleDestroyed();
	}
}

void ARANexus::HandleDestroyed()
{
	if (bIsDestroyed)
	{
		return;
	}

	bIsDestroyed = true;
	OnNexusDestroyed.Broadcast();

	if (URAWaveSubSystem* WaveSubSystem = GetWorld()->GetSubsystem<URAWaveSubSystem>())
	{
		WaveSubSystem->EndWave(ERAWaveEndReason::NexusDestroyed);
	}
}
