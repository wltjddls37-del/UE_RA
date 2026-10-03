// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/RABaseStatRow.h"
#include "RANexus.generated.h"

class UStaticMeshComponent;
class URAStatComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNexusDestroyed);

UCLASS()
class RA_API ARANexus : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARANexus();

	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Nexus")
	URAStatComponent* GetStatComponent() const { return StatComponent; }

	UFUNCTION(BlueprintPure, Category = "Nexus")
	bool IsDestroyed() const { return bIsDestroyed; }

	UPROPERTY(BlueprintAssignable, Category = "Nexus")
	FOnNexusDestroyed OnNexusDestroyed;

private:
	UFUNCTION()
	void HandleStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue);

	// 넥서스 파괴 시 웨이브 종료 (패배)
	void HandleDestroyed();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<URAStatComponent> StatComponent;

private:
	bool bIsDestroyed = false;
};
