// Fill out your copyright notice in the Description page of Project Settings.



#include "Character/RAPlayerProjectile.h" // 본인 헤더를 항상 맨 처음에 작성한다 (기억해)
//헤더에서 전방 선언만 했던 클래스들 -> 실제로 쓰려면 여기서 include 해야 함
#include "Components/SphereComponent.h"
#include	"Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"// Apply Damage 를 위해 필요
#include "Kismet/GameplayStatics.h" // Apply Damage 를 위해 필요
#include "Monster/RAMonster.h" // 맞은 대상이 몬스터인지 확인
#include "Character/RAPlayer.h" // 킬 카운트(Addkill)

// Sets default values
ARAPlayerProjectile::ARAPlayerProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	//움직임은 ProjectileMovementComponent 가 담당하므로 Tick()은 꺼도 된다(성능)

	PrimaryActorTick.bCanEverTick = false;

	//CreateDefaultSubobject<타입>(이름) : 생성자에서 컴포넌트 만드는 방법
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(20.0f); // 반지름 20(총알 판정 크기)
	SetRootComponent(CollisionComponent);	//이 구가 액터의 중심(루트)

	//충돌 설정
	//QueryOnly : 물리로 밀어내지는 않고, " 겹쳤나?" 만 검사만 함
	//일단 모든 채널을 무시(Ignore)하고, Pawn몬스터캡슐만 겹침(OVerlap)으로 설정
	// -> 몬스터에 닿으면 HandleOverlap() 호출됨

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// 바닥 / 벽(WorldStatic)은 막힘(Block) → ProjectileMovement 가 멈추고 OnProjectileStop 호출
	// (크로스헤어로 아래를 조준하면 총알이 바닥에 박혀야 하니까)
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->SetGenerateOverlapEvents(true); // Overlap 이벤트 켜기

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CollisionComponent); // 루트(구) 에 붙힘
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌은 구만 담당

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	MovementComponent->UpdatedComponent = CollisionComponent; // 이 컴포넌트가 움직임을 담당
	MovementComponent->InitialSpeed = 3000.f; // 스폰 될 때 바라보는 방향으로 이속도로 출발
	MovementComponent->MaxSpeed = 3000.f; // 최대 속도
	MovementComponent->ProjectileGravityScale = 0.f; // 중력 0 -> 영향 없고 떨어지지않고 직선으로 간다.
	MovementComponent->bRotationFollowsVelocity = true; // 날아가는 방향을 바라본다.



}

// Called when the game starts or when spawned
void ARAPlayerProjectile::BeginPlay()
{
	Super::BeginPlay(); // 부모 BeginPlay를 꼭 먼저 호출해라

	// LifeTime 초 뒤에 자동으로 Destroy(아무것도 못 맞혔을때 정리)
	SetLifeSpan(LifeTime);
	
	// 쏜 사람(Owner = 플레이어) 과는 겹치지 않게 무시
	if (AActor* Shooter = GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(Shooter,true);

	}

	// 델리게이트 연결 : 구가 무언가와 겹치기 시작하면 HandleOverlap() 호출
	// AddDyamic(누가, & 클래스:: 함수 ) 

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, & ARAPlayerProjectile::HandleOverlap);

	// 바닥/벽에 막혀서 멈추면 HandleStop() 호출
	MovementComponent->OnProjectileStop.AddDynamic(this, &ARAPlayerProjectile::HandleStop);



}


void ARAPlayerProjectile::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ★ 이미 사라지는 중이면 무시
	//   (몬스터의 캡슐과 메시에 동시에 닿아서 2번 호출되는 경우 방지)
	if (IsActorBeingDestroyed())
	{
		return;
	}

	//몬스터가 아니면(플레이어 등 ) 무시 하고 계속 날아감 
	//Cast<T>() ; 맞으면 그 타입 포인터, 아니면 nullptr 반환

	ARAMonster* Monster = Cast<ARAMonster>(OtherActor);
	if (Monster == nullptr || Monster->IsDead())

	{

		return;

	}

	// 쏜 플레이어 ( 킬 카운트 , 데미지 가해자 정보용)
	ARAPlayer* Player = Cast<ARAPlayer>(GetOwner());

	//때리기 전에 살아 있는지 확인 -> 이번 공격으로 죽었는지 판단

	const bool bWasAlive = Monster->IsDead() == false;

	//데미지 주기 -> 몬스터의 TakeDamage() 호출됨

	UGameplayStatics::ApplyDamage(Monster, Damage, Player ? Player->GetController() : nullptr, this, nullptr);

	// 이번 총알로 죽었으면 킬 +1 (10킬 강화)

	if (Player && bWasAlive && Monster->IsDead())
	{
		Player->AddKill();
	}

	//맞혔으니 총알 제거
	Destroy();


}

void ARAPlayerProjectile::HandleStop(const FHitResult& ImpactResult)
{
	// 바닥이나 벽에 박혔으니 총알 제거
	Destroy();
}
