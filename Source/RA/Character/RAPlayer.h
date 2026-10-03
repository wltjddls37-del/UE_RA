// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RACharacter.h"
#include "RAPlayer.generated.h"

class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class UInputAction;
struct  FInputActionValue;

UCLASS()
class RA_API ARAPlayer : public ARACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARAPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void Move(const FInputActionValue& InValue);
	void Look(const FInputActionValue& InValue);
	void Attack();

private:
	void PlayNextAttackMontage();
	void ResetAttack();

	UFUNCTION()
	void HandleMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

protected:
	UPROPERTY(EditAnywhere, Category = "RA")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "RA")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "RA")
	TObjectPtr<UInputAction> LookAction;
		
	UPROPERTY(EditAnywhere, Category = "RA")
	TObjectPtr<UInputAction> AttackAction;

	// 공격 입력마다 Index 순서대로 재생
	UPROPERTY(EditAnywhere, Category = "RA")
	TArray<TObjectPtr<UAnimMontage>> AttackMontageArray;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent>	SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

private:
	// 재생 중인 공격 몽타주, 없으면 nullptr
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAttackMontage;

	// 다음에 재생할 AttackMontageArray Index
	int32 AttackIndex = 0;

	// 재생 중에 들어온 공격 입력, 현재 몽타주가 끝나면 다음 몽타주 재생
	bool bAttackInputQueued = false;
};
