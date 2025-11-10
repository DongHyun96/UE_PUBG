// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomNavLinkProxy/C_CustomNavLinkProxy.h"

#include "C_NavLinkJumpStrategy.h"
#include "C_NavLinkParkourStrategy.h"

#include "NavLinkCustomComponent.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"

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
	TryInitLinkActionStrategies();

	FVector LeftPointLocation = GetTransform().TransformPosition(PointLinks[0].Left);
	FVector RightPointLocation = GetTransform().TransformPosition(PointLinks[0].Right);
	DestinationXY[EDirection::LEFT_TO_RIGHT] = UC_Util::GetXY(RightPointLocation);
	DestinationXY[EDirection::RIGHT_TO_LEFT] = UC_Util::GetXY(LeftPointLocation);
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

	CustomNavLinkProxyCount = 0;
	
	if (!LinkActionStrategies.IsEmpty()) LinkActionStrategies.Empty(); // GC는 GameSceneManager에서 처리 예정
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

	HandleEnteredEnemiesDestArrival();
}

void AC_CustomNavLinkProxy::HandleEnteredEnemiesDestArrival()
{
	for (int i = 0; i < static_cast<int>(EDirection::MAX); ++i)
	{
		EDirection Direction = static_cast<EDirection>(i);
		if (LinkEnteredEnemies[Direction].IsEmpty()) continue;

		FVector2D Destination			= DestinationXY[Direction];
		TSet<AC_Enemy*>& EnteredEnemies = LinkEnteredEnemies[Direction];

		for (auto It = EnteredEnemies.CreateIterator(); It; ++It)
		{
			AC_Enemy* Enemy = *It;

			// 이동 중 사망한 Enemy 체크
			if (Enemy->GetMainState() == EMainState::DEAD)
			{
				It.RemoveCurrent();
				continue;
			}
			
			FVector2D XYPos = UC_Util::GetXY(Enemy->GetActorLocation());
			if (FVector2D::Distance(Destination, XYPos) > 20.f) continue;

			// Reach End Point
			UC_Util::Print("End Point Reached!", GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);

			// Set to Dest pose State
			Enemy->SetPoseState(Enemy->GetPoseState(), DirectionPoseStates[Direction].DestPointPoseState);
			
			It.RemoveCurrent(); // Destination 처리 완료한 인원 제거
		}
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
	
	// 방향 확인하기
	FVector LeftWorldLocation	= GetTransform().TransformPosition(PointLinks[0].Left);
	float LeftToDest			= FVector::Distance(LeftWorldLocation, Destination);
	FVector RightWorldLocation	= GetTransform().TransformPosition(PointLinks[0].Right);
	float RightToDest			= FVector::Distance(RightWorldLocation, Destination);
	
	EDirection CurDirection = (LeftToDest > RightToDest) ? EDirection::LEFT_TO_RIGHT : EDirection::RIGHT_TO_LEFT;
	FVector StartLocation	= (CurDirection == EDirection::LEFT_TO_RIGHT) ? LeftWorldLocation : RightWorldLocation;
	FString str = "OnReceiveSmartLinkReached";
	str += (CurDirection == EDirection::LEFT_TO_RIGHT) ? " : Left" : " : Right";
	UC_Util::Print(str, GAMESCENE_MANAGER->GetTickRandomColor(), 10.f);
	// UC_Util::Print((CurDirection == EDirection::RIGHT_TO_LEFT) ? "Right_To_Left" : "Left_To_Right", FColor::MakeRandomColor(), 10.f);

	// 시작 지점 자세 변환 적용, 이미 같은 자세라면 전환 x
	Enemy->SetPoseState(Enemy->GetPoseState(), DirectionPoseStates[CurDirection].StartPointPoseState);
	
	FTimerHandle& TimerHandle = GAMESCENE_MANAGER->GetTimerHandle();
	
	// 도착 지점 확인 Set에 Enemy 넣어두기
	LinkEnteredEnemies[CurDirection].Add(Enemy);

	// 불가피하게 도착지점에 도착 못할 수 있을 때 인원체크 set에서 삭제
	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle,
		[this, Enemy, CurDirection]() { LinkEnteredEnemies[CurDirection].Remove(Enemy); },
		20.f,
		false
	);

	/* NavLinkProxy Strategy handling */
	
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

		// 정확한 처리를 위해서 Enemy StartPoint로 위치 지정
		Enemy->SetActorBottomLocation(StartLocation, ETeleportType::ResetPhysics);
		
		LinkActionStrategies[DirectionActionStrategies[CurDirection]]->ExecuteStartPointAction(this, Enemy, StartLocation, CurDirection);
	}
}

