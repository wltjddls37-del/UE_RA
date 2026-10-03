// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/RAMonster.h"
#include "Components/WidgetComponent.h"
#include "Component/RAStatComponent.h"
#include "UI/RAMonsterHealthBarWidget.h"

// Sets default values
ARAMonster::ARAMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	// 스포너에서 생성되었을 때도 AIController 가 빙의하도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(100.f, 10.f));
	HealthBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARAMonster::BeginPlay()
{
	// 위젯은 Super::BeginPlay 에서 WidgetComponent 가 생성
	Super::BeginPlay();

	if (URAMonsterHealthBarWidget* Widget = Cast<URAMonsterHealthBarWidget>(HealthBarWidget->GetUserWidgetObject()))
	{
		StatComponent->OnUpdateStat.AddDynamic(Widget, &URAMonsterHealthBarWidget::UpdateStat);

		// StatComponent 는 이미 초기화되었으므로 현재 체력 반영
		Widget->UpdateStat(ERAStatType::Health, StatComponent->GetValue(ERAStatType::Health), StatComponent->GetBaseValue(ERAStatType::Health));
	}
}

void ARAMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Die 를 거치지 않고 제거된 경우(낙사 등)에도 사망으로 알려 웨이브가 끝나지 않는 문제를 막는다
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		Die();
	}

	// WidgetComponent 가 위젯을 해제하기 전(Super::EndPlay)에 바인딩 해제
	if (URAMonsterHealthBarWidget* Widget = Cast<URAMonsterHealthBarWidget>(HealthBarWidget->GetUserWidgetObject()))
	{
		StatComponent->OnUpdateStat.RemoveDynamic(Widget, &URAMonsterHealthBarWidget::UpdateStat);
	}

	Super::EndPlay(EndPlayReason);
}
