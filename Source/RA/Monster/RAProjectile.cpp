// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/RAProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ARAProjectile::ARAProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(15.f);
	SetRootComponent(CollisionComponent);

	// 벽, 바닥, 넥서스 등에 부딪히고 Pawn(몬스터, 플레이어)은 통과
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TrailComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail"));
	TrailComponent->SetupAttachment(CollisionComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	MovementComponent->UpdatedComponent = CollisionComponent;
	MovementComponent->InitialSpeed = 1500.f;
	MovementComponent->MaxSpeed = 1500.f;
	MovementComponent->ProjectileGravityScale = 0.f;
	MovementComponent->bRotationFollowsVelocity = true;
}

void ARAProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeTime);

	// 쏜 몬스터 자신은 무시
	if (AActor* Shooter = GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(Shooter, true);
	}

	CollisionComponent->OnComponentHit.AddDynamic(this, &ARAProjectile::HandleHit);
}

void ARAProjectile::HandleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != GetOwner())
	{
		APawn* Shooter = Cast<APawn>(GetOwner());
		UGameplayStatics::ApplyDamage(OtherActor, Damage, Shooter ? Shooter->GetController() : nullptr, this, nullptr);
	}

	Destroy();
}
