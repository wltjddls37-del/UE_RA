// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/RAAnimNotify_FireProjectile.h"   // 자기 헤더를 항상 맨 처음에

// 헤더에서 전방 선언만 했던 클래스 + 이 파일에서 쓰는 클래스들
#include "Character/RAPlayer.h"                 // 쏘는 사람 (플레이어)
#include "Character/RAPlayerProjectile.h"       // 발사할 총알
#include "Component/RAStatComponent.h"          // 공격력 가져오기
#include "Components/SkeletalMeshComponent.h"   // 소켓 위치 가져오기
#include "Engine/World.h"                       // SpawnActor

void URAAnimNotify_FireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// 부모 Notify 먼저 호출
	Super::Notify(MeshComp, Animation, EventReference);

	// ─────────────────────────────
	// 0) 쏠 수 있는 상황인지 확인
	// ─────────────────────────────
	// MeshComp 의 주인(Owner) = 이 애니메이션을 재생 중인 캐릭터
	// 몽타주 미리보기 창에서는 주인이 플레이어가 아니라서 nullptr → 아무것도 안 함
	ARAPlayer* Player = MeshComp ? Cast<ARAPlayer>(MeshComp->GetOwner()) : nullptr;
	if (Player == nullptr)
	{
		return;
	}

	// 몽타주 노티파이에 총알 BP 를 지정 안 했으면 쏠 게 없음
	if (ProjectileClass == nullptr)
	{
		return;
	}

	UWorld* World = Player->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	// ─────────────────────────────
	// 1) 캐릭터 몸 돌리기: 카메라가 보는 방향(좌우만)
	// ─────────────────────────────
	// GetControlRotation() = 마우스로 돌린 카메라(컨트롤러) 회전
	// 몸은 좌우(Yaw)만 돌린다 → 위아래(Pitch)까지 돌리면 캐릭터가 앞/뒤로 기울어짐
	const FRotator BodyRotation(0.f, Player->GetControlRotation().Yaw, 0.f);

	// ★ 위치(소켓)를 구하기 "전에" 몸을 먼저 돌린다
	//   몸이 돌아가야 총구 소켓 위치도 같이 돌아간 위치로 계산됨
	Player->SetActorRotation(BodyRotation);

	// ─────────────────────────────
	// 2) 위치 정하기: 소켓 또는 캐릭터 앞쪽
	// ─────────────────────────────
	FVector SpawnLocation;

	// 소켓 이름을 적었고, 메시에 그 소켓이 실제로 있으면 → 소켓 위치 (예: 총구)
	if (SocketName.IsNone() == false && MeshComp->DoesSocketExist(SocketName))
	{
		SpawnLocation = MeshComp->GetSocketLocation(SocketName);
	}
	else
	{
		// 없으면 → 캐릭터 위치 + 오프셋
		// ★ RotateVector: 오프셋 (100,0,50) 을 "캐릭터가 보는 방향 기준"으로 돌려줌
		//   (그냥 더하면 월드 X축 기준이라 캐릭터가 어디를 보든 같은 쪽에서 나감)
		SpawnLocation = Player->GetActorLocation() + BodyRotation.RotateVector(SpawnOffset);
	}

	// ─────────────────────────────
	// 3) 방향 정하기: 총구 → 크로스헤어가 가리키는 지점
	// ─────────────────────────────
	// TraceAim: 화면 중앙으로 라인트레이스 → 맞은 곳(또는 사거리 끝)이 ImpactPoint
	FHitResult AimHit;
	Player->TraceAim(AimHit);
	const FVector TargetLocation = AimHit.ImpactPoint;

	// 목표 - 시작 = 시작에서 목표로 가는 벡터 → 정규화하면 "방향"
	FVector AimDirection = (TargetLocation - SpawnLocation).GetSafeNormal();

	// ★ 예외: 목표가 총구보다 뒤에 있거나 너무 가까우면 (벽에 딱 붙어서 쏠 때)
	//   총알이 뒤로 날아가는 걸 막기 위해 그냥 카메라 정면 방향으로 쏜다
	//   DotProduct < 0 → 두 방향이 90도 넘게 벌어짐 (= 반대쪽)
	const FVector CameraForward = Player->GetControlRotation().Vector();
	if (AimDirection.IsNearlyZero() || FVector::DotProduct(AimDirection, CameraForward) < 0.f)
	{
		AimDirection = CameraForward;
	}

	// 방향 벡터 → 회전값 (총알의 앞방향이 이 방향을 보게)
	const FRotator AimRotation = AimDirection.Rotation();


	// ─────────────────────────────
	// 4) 총알 스폰
	// ─────────────────────────────
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Player;        // ★ 총알의 GetOwner() = 플레이어 → 총알이 쏜 사람 무시 + 킬 카운트
	SpawnParams.Instigator = Player;   // 데미지를 일으킨 Pawn
	// 스폰 위치에 뭔가 겹쳐 있어도 무조건 생성
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// SpawnActor<부모타입>(실제 생성할 클래스, 위치, 회전, 파라미터)
	// → 회전 방향 = 총알의 앞방향 → ProjectileMovement 가 그 방향으로 InitialSpeed 로 날려줌
	ARAPlayerProjectile* Projectile = World->SpawnActor<ARAPlayerProjectile>(ProjectileClass, SpawnLocation, AimRotation, SpawnParams);
	if (Projectile == nullptr)
	{
		return;
	}

	// ─────────────────────────────
	// 5) 데미지 넘겨주기: 플레이어 공격력
	// ─────────────────────────────
	// (10킬 강화로 공격력이 오르면 총알 데미지도 같이 오름)
	const float Damage = Player->GetStatComponent()->GetValue(ERAStatType::AttackPower);
	Projectile->SetDamage(Damage);
}
