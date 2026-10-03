// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RAPlayerController.generated.h"

class URAPlayerHUDWidget;

UCLASS(Config = Game)
class RA_API ARAPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	URAPlayerHUDWidget* GetPlayerHUDWidget() const { return PlayerHUDWidget; }

protected:
	virtual void BeginPlay() override;

private:
	// DefaultGame.ini 의 [/Script/RA.RAPlayerController] 에서 설정
	UPROPERTY(Config)
	TSoftClassPtr<URAPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<URAPlayerHUDWidget> PlayerHUDWidget;
};
