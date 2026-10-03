// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/RAWaveSubSystem.h"
#include "Monster/RASpawner.h"
#include "Monster/RAMonster.h"
#include "TimerManager.h"

void URAWaveSubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// 액터 BeginPlay 보다 먼저 호출되지만, 타이머는 다음 프레임 이후에 실행되므로 스포너 등록이 끝난 뒤 첫 웨이브가 시작된다
	InWorld.GetTimerManager().SetTimer(WaveTimerHandle, this, &URAWaveSubSystem::StartNextWave, WaveInterval, true, FirstWaveDelay);
}

void URAWaveSubSystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaveTimerHandle);
	}

	SpawnerArray.Empty();

	Super::Deinitialize();
}

bool URAWaveSubSystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void URAWaveSubSystem::RegisterSpawner(ARASpawner* Spawner)
{
	if (Spawner)
	{
		SpawnerArray.AddUnique(Spawner);
	}
}

void URAWaveSubSystem::UnregisterSpawner(ARASpawner* Spawner)
{
	SpawnerArray.Remove(Spawner);
}

int32 URAWaveSubSystem::GetTotalWaveCount() const
{
	int32 TotalWaveCount = 0;
	for (const ARASpawner* Spawner : SpawnerArray)
	{
		if (Spawner)
		{
			TotalWaveCount = FMath::Max(TotalWaveCount, Spawner->GetWaveCount());
		}
	}
	return TotalWaveCount;
}

void URAWaveSubSystem::RegisterMonster(ARAMonster* Monster)
{
	if (!Monster || Monster->IsDead())
	{
		return;
	}

	++AliveMonsterCount;
	Monster->OnCharacterDied.AddDynamic(this, &URAWaveSubSystem::HandleMonsterDied);
}

void URAWaveSubSystem::HandleMonsterDied(ARACharacter* Monster)
{
	if (Monster)
	{
		Monster->OnCharacterDied.RemoveDynamic(this, &URAWaveSubSystem::HandleMonsterDied);
	}

	AliveMonsterCount = FMath::Max(AliveMonsterCount - 1, 0);

	if (AliveMonsterCount == 0)
	{
		EndWave(ERAWaveEndReason::AllMonstersDead);
	}
}

void URAWaveSubSystem::EndWave(ERAWaveEndReason Reason)
{
	if (bIsGameOver)
	{
		return;
	}

	// 넥서스 파괴는 웨이브 진행 여부와 관계없이 게임 종료
	if (Reason == ERAWaveEndReason::AllMonstersDead && !bIsWaveInProgress)
	{
		return;
	}

	bIsWaveInProgress = false;

	if (Reason == ERAWaveEndReason::NexusDestroyed || CurrentWave >= GetTotalWaveCount())
	{
		bIsGameOver = true;
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
	}

	OnWaveEnded.Broadcast(CurrentWave, Reason);
}

void URAWaveSubSystem::StartNextWave()
{
	if (bIsGameOver)
	{
		return;
	}

	if (CurrentWave >= GetTotalWaveCount())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
		return;
	}

	const int32 WaveIndex = CurrentWave;
	++CurrentWave;
	bIsWaveInProgress = true;

	for (ARASpawner* Spawner : SpawnerArray)
	{
		if (Spawner)
		{
			Spawner->SpawnWave(WaveIndex);
		}
	}

	OnWaveStarted.Broadcast(CurrentWave);

	// 마지막 웨이브를 시작했으면 타이머 종료
	if (CurrentWave >= GetTotalWaveCount())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
	}

	// 생성된 몬스터가 없으면 바로 종료
	if (AliveMonsterCount == 0)
	{
		EndWave(ERAWaveEndReason::AllMonstersDead);
	}
}
