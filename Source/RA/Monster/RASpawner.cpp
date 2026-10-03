// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/RASpawner.h"
#include "Monster/RAMonster.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Subsystem/RAWaveSubSystem.h"
#include "Subsystem/RAGameDataSubsystem.h"
#include "Data/RASpawnRow.h"

// Sets default values
ARASpawner::ARASpawner()
{
	// 웨이브는 WaveSubSystem 타이머로 처리하므로 Tick 불필요
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ARASpawner::BeginPlay()
{
	Super::BeginPlay();

	if (URAWaveSubSystem* WaveSubSystem = GetWorld()->GetSubsystem<URAWaveSubSystem>())
	{
		WaveSubSystem->RegisterSpawner(this);
	}
}

void ARASpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URAWaveSubSystem* WaveSubSystem = GetWorld()->GetSubsystem<URAWaveSubSystem>())
	{
		WaveSubSystem->UnregisterSpawner(this);
	}

	Super::EndPlay(EndPlayReason);
}

const FRASpawnRow* ARASpawner::GetSpawnData() const
{
	const URAGameDataSubsystem* GameDataSubsystem = UGameInstance::GetSubsystem<URAGameDataSubsystem>(GetGameInstance());
	return GameDataSubsystem ? GameDataSubsystem->GetSpawnData(SpawnRowName) : nullptr;
}

int32 ARASpawner::GetWaveCount() const
{
	const FRASpawnRow* SpawnData = GetSpawnData();
	return SpawnData ? SpawnData->WaveInfoArray.Num() : 0;
}

void ARASpawner::SpawnWave(int32 WaveIndex)
{
	const FRASpawnRow* SpawnData = GetSpawnData();
	if (!SpawnData || !SpawnData->WaveInfoArray.IsValidIndex(WaveIndex))
	{
		return;
	}

	for (const TPair<TSubclassOf<ARAMonster>, int32>& Pair : SpawnData->WaveInfoArray[WaveIndex].MonsterCountMap)
	{
		for (int32 i = 0; i < Pair.Value; ++i)
		{
			SpawnMonster(Pair.Key);
		}
	}
}

void ARASpawner::SpawnMonster(TSubclassOf<ARAMonster> MonsterClass)
{
	if (!MonsterClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ARAMonster* Monster = GetWorld()->SpawnActor<ARAMonster>(MonsterClass, GetRandomSpawnLocation(), GetActorRotation(), SpawnParams);
	if (!Monster)
	{
		return;
	}

	if (URAWaveSubSystem* WaveSubSystem = GetWorld()->GetSubsystem<URAWaveSubSystem>())
	{
		WaveSubSystem->RegisterMonster(Monster);
	}
}

FVector ARASpawner::GetRandomSpawnLocation() const
{
	const FVector2D Offset = FMath::RandPointInCircle(SpawnRadius);
	return GetActorLocation() + FVector(Offset.X, Offset.Y, 100.f);
}
