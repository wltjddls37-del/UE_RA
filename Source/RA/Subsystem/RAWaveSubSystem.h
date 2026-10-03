// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RAWaveSubSystem.generated.h"

class ARASpawner;
class ARAMonster;
class ARACharacter;

UENUM(BlueprintType)
enum class ERAWaveEndReason : uint8
{
	AllMonstersDead,
	NexusDestroyed,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveEnded, int32, WaveNumber, ERAWaveEndReason, Reason);

UCLASS(Config = Game)
class RA_API URAWaveSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

	void RegisterSpawner(ARASpawner* Spawner);
	void UnregisterSpawner(ARASpawner* Spawner);

	// 스포너가 몬스터를 생성할 때 호출, 살아있는 몬스터 수를 관리한다
	void RegisterMonster(ARAMonster* Monster);

	// 웨이브 종료, 넥서스가 파괴되거나 살아있는 몬스터가 모두 죽으면 호출된다
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void EndWave(ERAWaveEndReason Reason);

	// 1부터 시작, 아직 웨이브가 시작되지 않았으면 0
	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetCurrentWave() const { return CurrentWave; }

	// 등록된 스포너 중 가장 많은 웨이브 수
	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetTotalWaveCount() const;

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetAliveMonsterCount() const { return AliveMonsterCount; }

	// 넥서스 파괴 또는 마지막 웨이브 종료 시 true
	UFUNCTION(BlueprintPure, Category = "Wave")
	bool IsGameOver() const { return bIsGameOver; }

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveStarted OnWaveStarted;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveEnded OnWaveEnded;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	void StartNextWave();

	UFUNCTION()
	void HandleMonsterDied(ARACharacter* Monster);

	// 웨이브 사이 간격 (초), DefaultGame.ini 에서 변경 가능
	UPROPERTY(Config)
	float WaveInterval = 180.f;

	// 게임 시작 후 첫 웨이브까지 대기 시간 (초)
	UPROPERTY(Config)
	float FirstWaveDelay = 10.f;

	UPROPERTY()
	TArray<TObjectPtr<ARASpawner>> SpawnerArray;

	int32 CurrentWave = 0;

	int32 AliveMonsterCount = 0;

	bool bIsWaveInProgress = false;

	bool bIsGameOver = false;

	FTimerHandle WaveTimerHandle;
};
