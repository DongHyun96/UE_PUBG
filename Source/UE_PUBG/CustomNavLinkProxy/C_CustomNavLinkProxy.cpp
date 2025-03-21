// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomNavLinkProxy/C_CustomNavLinkProxy.h"

#include "C_NavLinkJumpStrategy.h"
#include "C_NavLinkParkourStrategy.h"
#include "NavigationSystem.h"
#include "NavLinkCustomComponent.h"
#include "StaticMeshAttributes.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"

#include "AI/NavigationSystemBase.h"
#include "Character/C_Enemy.h"
#include "Character/Component/C_ParkourComponent.h"
#include "Components/SphereComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Singleton/C_GameSceneManager.h"

#include "Utility/C_Util.h"

int AC_CustomNavLinkProxy::CustomNavLinkProxyCount = 0;
TMap<ELinkActionStrategy, class II_NavLinkProxyActionStrategy*> AC_CustomNavLinkProxy::LinkActionStrategies{};


AC_CustomNavLinkProxy::AC_CustomNavLinkProxy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_CustomNavLinkProxy::BeginPlay()
{
	Super::BeginPlay();

	++CustomNavLinkProxyCount;

	InitLeftRightColliderSphere();
	TryInitLinkActionStrategies();
	
}

void AC_CustomNavLinkProxy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Type::Destroyed)
	{
		if (--CustomNavLinkProxyCount <= 0)
		{
			if (!LinkActionStrategies.IsEmpty()) LinkActionStrategies.Empty(); // GC는 GameSceneManager에서 처리 예정
		}
		return;
	}
	if (!LinkActionStrategies.IsEmpty()) LinkActionStrategies.Empty(); // GC는 GameSceneManager에서 처리 예정
}

void AC_CustomNavLinkProxy::InitLeftRightColliderSphere()
{
	LeftSphereCollider  = Cast<UShapeComponent>(GetDefaultSubobjectByName("LeftSphereCollision"));
	RightSphereCollider = Cast<UShapeComponent>(GetDefaultSubobjectByName("RightSphereCollision"));

	if (!IsValid(LeftSphereCollider) || !IsValid(RightSphereCollider))
	{
		UC_Util::Print("From AC_CustomNavLinkProxy::BeginPlay() : Sphere Colliders not inited properly!", FColor::Red, 10.f);
		return;
	}

	FVector LeftPointLinkLocation = GetTransform().TransformPosition(PointLinks[0].Left);
	FVector RightPointLinkLocation = GetTransform().TransformPosition(PointLinks[0].Right);

	FNavLocation NavLocation{};

	bool Founded = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->ProjectPointToNavigation
	(
		LeftPointLinkLocation,
		NavLocation,
		FVector(100.f, 100.f, 100.f)
	);

	FColor RandomColor = FColor::MakeRandomColor();

	if (!Founded) UC_Util::Print("From " + GetActorLabel() + " : LeftCollider location not properly inited!", RandomColor, 10.f);
	LeftSphereCollider->SetWorldLocation(NavLocation.Location);
	
	NavLocation = {};
	Founded = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->ProjectPointToNavigation
		(
			RightPointLinkLocation,
			NavLocation,
			FVector(100.f, 100.f, 100.f)
		);

	if (!Founded) UC_Util::Print("From " + GetActorLabel() + " : RightCollider location not properly inited!", RandomColor, 10.f);
	RightSphereCollider->SetWorldLocation(NavLocation.Location);

	LeftSphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AC_CustomNavLinkProxy::OnLeftSphereBeginOverlap);
	RightSphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AC_CustomNavLinkProxy::OnRightSphereBeginOverlap);

	SetActorEnableCollision(true);
	LeftSphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightSphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

bool AC_CustomNavLinkProxy::TryInitLinkActionStrategies()
{
	if (!LinkActionStrategies.IsEmpty()) return false;
	
	II_NavLinkProxyActionStrategy* LinkJumpStrategy = NewObject<UC_NavLinkJumpStrategy>();
	LinkJumpStrategy->_getUObject()->AddToRoot(); // GC 방지

	II_NavLinkProxyActionStrategy* LinkParkourStrategy = NewObject<UC_NavLinkParkourStrategy>();
	LinkParkourStrategy->_getUObject()->AddToRoot();

	GAMESCENE_MANAGER->AddGCProtectedObject(LinkJumpStrategy->_getUObject());
	GAMESCENE_MANAGER->AddGCProtectedObject(LinkParkourStrategy->_getUObject());

	LinkActionStrategies.Add(ELinkActionStrategy::DEFAULT, nullptr);
	LinkActionStrategies.Add(ELinkActionStrategy::MAX, nullptr);
	LinkActionStrategies.Add(ELinkActionStrategy::JUMP, LinkJumpStrategy);
	LinkActionStrategies.Add(ELinkActionStrategy::PARKOUR, LinkParkourStrategy);
	return true;
}

void AC_CustomNavLinkProxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*switch (LeftSphereCollider->GetCollisionEnabled())
	{
	case ECollisionEnabled::NoCollision: UC_Util::Print("NoCollision", FColor::Red, 10.f); break;;
	case ECollisionEnabled::QueryOnly: UC_Util::Print("QueryOnly", FColor::Red, 10.f); break;
	case ECollisionEnabled::PhysicsOnly: UC_Util::Print("PhysicsOnly", FColor::Red, 10.f); break;
	case ECollisionEnabled::QueryAndPhysics: UC_Util::Print("QueryAndPhysics", FColor::Red, 10.f); break;
	case ECollisionEnabled::ProbeOnly: UC_Util::Print("ProbeOnly", FColor::Red, 10.f); break;
	case ECollisionEnabled::QueryAndProbe: UC_Util::Print("QueryAndProbe", FColor::Red, 10.f); break;
	}*/

	if (LeftSphereCollider->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
	{
		UC_Util::Print("Setting!");
		LeftSphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if (RightSphereCollider->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
	{
		RightSphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		
	}
}

void AC_CustomNavLinkProxy::OnReceiveSmartLinkReached(AActor* Agent, const FVector& Destination)
{
	// 도착지점에선 Callback이 안들어오는 중
	
	AC_Enemy* Enemy = Cast<AC_Enemy>(Agent); if (!Enemy) return;

	if (PointLinks.IsEmpty())
	{
		UC_Util::Print("From AC_CustomNavLinkProxy::OnReceiveSmartLinkReached : PointLinks is empty!", FColor::Red, 10.f);
		return;
	}
	UC_Util::Print("OnReceiveSmartLinkReached", FColor::Red, 10.f);
	
	// 방향 확인하기
	FVector LeftWorldLocation	= GetTransform().TransformPosition(PointLinks[0].Left);
	float LeftToDest			= FVector::Distance(LeftWorldLocation, Destination);
	FVector RightWorldLocation	= GetTransform().TransformPosition(PointLinks[0].Right);
	float RightToDest			= FVector::Distance(RightWorldLocation, Destination);
	
	EDirection CurDirection = (LeftToDest > RightToDest) ? EDirection::LEFT_TO_RIGHT : EDirection::RIGHT_TO_LEFT;
	FVector StartLocation	= (CurDirection == EDirection::LEFT_TO_RIGHT) ? LeftWorldLocation : RightWorldLocation;
	// UC_Util::Print((CurDirection == EDirection::RIGHT_TO_LEFT) ? "Right_To_Left" : "Left_To_Right", FColor::MakeRandomColor(), 10.f);

	// 시작 지점 자세 변환 적용, 이미 같은 자세라면 전환 x
	if (Enemy->GetPoseState() != DirectionPoseStates[CurDirection].StartPointPoseState)
	{
		UC_BehaviorComponent* BehaviorComponent = Enemy->GetEnemyAIController()->GetBehaviorComponent();
		BehaviorComponent->SetNextPoseState(DirectionPoseStates[CurDirection].StartPointPoseState);
		BehaviorComponent->SetIdleTaskType(EIdleTaskType::CHANGE_POSE); // Service는 현재 MoveToTask 진행 중이였기 때문에 바꿀 필요 x	
	}
	
	FTimerHandle TimerHandle{};
	
	// 도착 지점 확인 Set에 Enemy 넣어두기
	if (CurDirection == EDirection::LEFT_TO_RIGHT)
	{
		RightSideDestEnemies.Add(Enemy, CurDirection);

		// 불가피하게 도착지점에 도착 못할 수 있을 때 인원체크 set에서 삭제
		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle,
			[this, Enemy]() {RightSideDestEnemies.Remove(Enemy);},
			20.f,
			false
		);
	}
	else
	{
		LeftSideDestEnemies.Add(Enemy, CurDirection);
		
		GetWorld()->GetTimerManager().SetTimer
		(
			TimerHandle,
			[this, Enemy]() {LeftSideDestEnemies.Remove(Enemy);},
			20.f,
			false
		);
	}

	if (DirectionActionStrategies[CurDirection] == ELinkActionStrategy::DEFAULT ||
		DirectionActionStrategies[CurDirection] == ELinkActionStrategy::MAX)
		return;

	// Strategy 적용
	if (LinkActionStrategies[DirectionActionStrategies[CurDirection]])
	{
		// Strategy를 사용할 지점으로 Enemy 회전
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation
		(
			Enemy->GetActorLocation(),
			Destination
		);

		LookAtRotation.Pitch = 0.f;
		LookAtRotation.Roll = 0.f;
		Enemy->SetActorRotation(LookAtRotation, ETeleportType::ResetPhysics);

		// 정확한 처리를 위해서 Enemy StartPoint로 위치 지정 -> TODO : StartPoint 위치말고 더 정확한 Strategy 위치 지정을 두어야 할 수도 있음
		Enemy->SetActorBottomLocation(StartLocation, ETeleportType::ResetPhysics);
		
		LinkActionStrategies[DirectionActionStrategies[CurDirection]]->ExecuteStartPointAction(this, Enemy, StartLocation);
	}
}

void AC_CustomNavLinkProxy::OnLeftSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UC_Util::Print("LeftBeginOverlap", FColor::Red, 10.f);

	AC_Enemy* Enemy = Cast<AC_Enemy>(OtherActor);
	if (!Enemy) return;

	if (!LeftSideDestEnemies.Contains(Enemy))
	{
		UC_Util::Print("LeftSideDest map does not contain overlapped Enemy", FColor::Red, 10.f);
		return;
	}
	EDirection CurDirection = LeftSideDestEnemies[Enemy];
	LeftSideDestEnemies.Remove(Enemy);
	
	// 도착지점 취할 자세 적용, 이미 같은 자세라면 전환 x
	if (Enemy->GetPoseState() == DirectionPoseStates[CurDirection].DestPointPoseState)
	{
		UC_Util::Print("Left side Enemy Overlapped but Same PoseState", FColor::Red, 10.f);
		return;
	}
		
	UC_BehaviorComponent* BehaviorComponent = Enemy->GetEnemyAIController()->GetBehaviorComponent();
	BehaviorComponent->SetNextPoseState(DirectionPoseStates[CurDirection].DestPointPoseState);
	BehaviorComponent->SetIdleTaskType(EIdleTaskType::CHANGE_POSE);
	
}

void AC_CustomNavLinkProxy::OnRightSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UC_Util::Print("RightBeginOverlap", FColor::Red, 10.f);
	
	AC_Enemy* Enemy = Cast<AC_Enemy>(OtherActor);
	if (!Enemy) return;

	if (!RightSideDestEnemies.Contains(Enemy))
	{
		UC_Util::Print("RightSideDest map does not contain overlapped Enemy", FColor::Red, 10.f);
		return;
	}
	EDirection CurDirection = RightSideDestEnemies[Enemy];
	RightSideDestEnemies.Remove(Enemy);
	
	// 도착지점 취할 자세 적용, 이미 같은 자세라면 전환 x
	if (Enemy->GetPoseState() == DirectionPoseStates[CurDirection].DestPointPoseState)
	{
		UC_Util::Print("Right side Enemy Overlapped but Same PoseState", FColor::Red, 10.f);
		return;
	}
		
	UC_BehaviorComponent* BehaviorComponent = Enemy->GetEnemyAIController()->GetBehaviorComponent();
	BehaviorComponent->SetNextPoseState(DirectionPoseStates[CurDirection].DestPointPoseState);
	BehaviorComponent->SetIdleTaskType(EIdleTaskType::CHANGE_POSE);
}
