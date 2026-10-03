// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RAPlayerController.h"
#include "UI/RAPlayerHUDWidget.h"

void ARAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 화면이 있는 로컬 플레이어만 HUD 생성
	if (IsLocalController() == false)
	{
		return;
	}

	const TSubclassOf<URAPlayerHUDWidget> HUDWidgetClass = PlayerHUDWidgetClass.LoadSynchronous();
	if (HUDWidgetClass == nullptr)
	{
		return;
	}

	PlayerHUDWidget = CreateWidget<URAPlayerHUDWidget>(this, HUDWidgetClass);
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->AddToViewport();
	}
}
