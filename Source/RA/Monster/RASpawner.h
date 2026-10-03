// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RASpawner.generated.h"

class UStaticMeshComponent;
class ARAMonster;
struct FRASpawnRow;

UCLASS()
class RA_API ARASpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARASpawner();

	// WaveSubSystem 이 웨이브를 시작할 때 호출, 이 스포너에 해당 웨이브 정보가 없으면 생성하지 않는다
	void SpawnWave(int32 WaveIndex);

	int32 GetWaveCount() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	const FRASpawnRow* GetSpawnData() const;
	void SpawnMonster(TSubclassOf<ARAMonster> MonsterClass);
	FVector GetRandomSpawnLocation() const;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	// 스폰 DataTable 의 RowName
	UPROPERTY(EditAnywhere, Category = "RA")
	FName SpawnRowName;

	// 스포너 중심으로부터 몬스터가 생성될 반경
	UPROPERTY(EditAnywhere, Category = "RA", meta = (ClampMin = "0.0"))
	float SpawnRadius = 300.f;
};
