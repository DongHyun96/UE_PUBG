// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/SkyDiveTask/C_BTTaskSkyDive.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "AI/Service/C_BTServiceIdle.h"
#include "Airplane/C_Airplane.h"
#include "Airplane/C_AirplaneManager.h"
#include "Singleton/C_GameSceneManager.h"

#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "Character/Component/SkyDivingComponent/C_EnemySkyDivingComponent.h"
#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"
#include "HUD/C_MainMapWidget.h"
#include "Utility/C_Util.h"

UC_BTTaskSkyDive::UC_BTTaskSkyDive()
{
	bNotifyTick = true;
}

void UC_BTTaskSkyDive::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController* EnemyAIController = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	if (!EnemyAIController)
	{
		UC_Util::Print("From UC_BTTaskSkyDive::TickTask : EnemyAIController Casting failed!", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AC_Enemy* Enemy = Cast<AC_Enemy>(EnemyAIController->GetPawn());
	UC_EnemySkyDivingComponent* EnemySkyDivingComponent = Cast<UC_EnemySkyDivingComponent>(Enemy->GetSkyDivingComponent());
	
	// SkyDiving이 잘 끝났다고 간주 IdleTask로 돌아가기 
	if (Enemy->GetMainState() != EMainState::SKYDIVING && !EnemySkyDivingComponent->IsCharacterLandingMontagePlaying())
	{
		EnemyAIController->GetBehaviorComponent()->SetServiceType(EServiceType::IDLE);
		EnemyAIController->GetBehaviorComponent()->SetIdleTaskType(EIdleTaskType::WAIT);	

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		
		UC_Util::Print
		(
			"From BTTaskSkyDive TickTask : Task completed due to Character MainState set to different State",
			FColor::MakeRandomColor(), 10.f
		);
		return;
	}


	if (EnemySkyDivingComponent->GetSkyDivingState() == ESkyDivingState::READY)
	{
		// Jump 위치 확인해서 도달했다면 Jump 하기
		FVector AirplaneLocation = GAMESCENE_MANAGER->GetAirplaneManager()->GetAirplane()->GetActorLocation();
		FVector2D AirplanePosXY = UC_Util::GetXY(AirplaneLocation);

		float DistanceToJumpLocation = FVector2D::Distance(AirplanePosXY, AirplaneJumpPosXYMap[Enemy]); 
		
		if (DistanceToJumpLocation < 500.f || SkyDiveStartFlagMap[Enemy])
		{
			SkyDiveStartFlagMap[Enemy] = true; // SKyDiving할 수 없는 상황일 수도 있기 때문에 다음 Tick에서 SkyDiving 다시 시도하는 장치 마련
			EnemySkyDivingComponent->SetSkyDivingState(ESkyDivingState::SKYDIVING);
		}
	}
}

EBTNodeResult::Type UC_BTTaskSkyDive::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	
	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskSkyDive ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	if (!IsValid(Enemy))
	{
		UC_Util::Print("From UC_BTTaskSkyDive ExecuteTask : OwnerEnemy Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	SkyDiveStartFlagMap.Add(Enemy, false);
	
	SetRandomLandingTargetLocationAndJumpPosXY(Enemy);	// SkyDiving 도착지점 세팅
	SetSkyDivingStateDestination(Enemy);				// SKyDiveState 상태일 때 도달할 지점 setting

	return EBTNodeResult::InProgress;
}

bool UC_BTTaskSkyDive::SetRandomLandingTargetLocationAndJumpPosXY(AC_Enemy* Enemy)
{
	FVector2D LandingTargetPos{};

	// 비행기 경로상으로부터 500m 이내의 Random한 지점을 LandingTargetLocation으로 지정할 예정
	static const float SKYDIVE_TRAVERSE_DISTANCE_LIMIT = 50000.f;
	
	const TPair<FVector, FVector> PlaneRouteStartDest	= GAMESCENE_MANAGER->GetAirplaneManager()->GetPlaneRouteStartDestPair();
	const FVector2D PlaneRouteStartPos					= UC_Util::GetXY(PlaneRouteStartDest.Key);
	const FVector2D PlaneRouteDestPos					= UC_Util::GetXY(PlaneRouteStartDest.Value);
	
	uint16 RandomTryCount{};
	FVector2D AirplaneJumpPosXY{};
	
	while (true)
	{
		TPair<uint8, uint8> RandomTileCoord = SKYDIVE_LANDING_TARGET_TILES[FMath::RandRange(0, SKYDIVE_LANDING_TARGET_TILES.Num() - 1)];

		FVector TileLocation = GAMESCENE_MANAGER->ConvertTileCoordinateToTileMiddleLocation(RandomTileCoord);
		float CellWorldSize  = GAMESCENE_MANAGER->GetCellWorldSize();

		float XOffset = FMath::RandRange(-CellWorldSize * 0.5f, CellWorldSize * 0.5f);
		float YOffset = FMath::RandRange(-CellWorldSize * 0.5f, CellWorldSize * 0.5f);

		LandingTargetPos = {TileLocation.X + XOffset, TileLocation.Y + YOffset};
		AirplaneJumpPosXY = GetClosestPointOnLine(LandingTargetPos, PlaneRouteStartPos, PlaneRouteDestPos); // AirplaneJumpPosXY 동시에 setting

		if (FVector2D::Distance(LandingTargetPos, AirplaneJumpPosXY) < SKYDIVE_TRAVERSE_DISTANCE_LIMIT ||
			++RandomTryCount > 500)
		{
			UC_Util::Print("From SettingLandingTargetLocationAndJumpPosXY : TryCount = " + FString::FromInt(RandomTryCount), FColor::Red, 10.f);
			break;
		}
	}

	AirplaneJumpPosXYMap.Add(Enemy, AirplaneJumpPosXY);

	// TODO : 밑 라인 단락 지우기 For Testing
	FVector JumpLocationTemp = { AirplaneJumpPosXY.X, AirplaneJumpPosXY.Y, 0.f };
	FVector LandingLocationTemp = {LandingTargetPos.X, LandingTargetPos.Y, 0.f };
	GAMESCENE_MANAGER->GetPlayer()->GetMainMapWidget()->SpawnJumpPosAndTargetPosImage(JumpLocationTemp, LandingLocationTemp);

	FVector LineTraceStartLocation = {LandingTargetPos.X, LandingTargetPos.Y, 40000.f}; // Random한 XY지점을 잡아 400m 상공에서 아래로 RayCasting할 예정 
	FVector LineTraceDestLocation  = LineTraceStartLocation - FVector::UnitZ() * 40000.f;
	
	// 상공에서 아래로 RayCasting해서 Hit한 지점을 Target Location으로 잡음
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(Enemy);
	FHitResult HitResult{};
	
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, LineTraceStartLocation, LineTraceDestLocation,
		ECollisionChannel::ECC_Visibility, CollisionParams);

	UC_BehaviorComponent* BehaviorComponent = Enemy->GetEnemyAIController()->GetBehaviorComponent();
	UC_EnemySkyDivingComponent* SkyDivingComponent = Cast<UC_EnemySkyDivingComponent>(Enemy->GetSkyDivingComponent());
	
	if (!HasHit)
	{
		// 거의 들어올 일 없을 예정
		UC_Util::Print("From UC_BTTaskSkyDive::SetRandomLandingTargetLocation : RayCasting to find Z failed! Setting alternative Z value.", FColor::Red, 10.f);
		FVector TargetLocation	= LineTraceStartLocation;
		TargetLocation.Z		= 3000.f;
		BehaviorComponent->SetBasicTargetLocation(TargetLocation);	// RayCasting 실패해도 임의의 높이의 지점으로 낙하지점위치 setting하기
		SkyDivingComponent->SetParachutingStateDestination(TargetLocation);
		return false;
	}

	BehaviorComponent->SetBasicTargetLocation(HitResult.ImpactPoint);
	SkyDivingComponent->SetParachutingStateDestination(HitResult.ImpactPoint);
	return true;
}

void UC_BTTaskSkyDive::SetSkyDivingStateDestination(AC_Enemy* Enemy)
{
	UC_EnemySkyDivingComponent* SkyDivingComponent = Cast<UC_EnemySkyDivingComponent>(Enemy->GetSkyDivingComponent());
	
	const float ParachuteDeployLimitHeight = UC_SkyDivingComponent::GetParachuteDeployLimitHeight();
	const FVector2D LandingTargetPosXY = UC_Util::GetXY(SkyDivingComponent->GetParachutingStateDestination());
	if (!AirplaneJumpPosXYMap.Contains(Enemy))
	{
		UC_Util::Print("From SetSkyDivingDestination : AirplaneJumpPosXYMap does not contain Enemy key!", FColor::Red, 10.f);
		return;
	}
	const FVector2D AirplaneJumpPosXY = AirplaneJumpPosXYMap[Enemy];
	const float JumpPosToLandingPosDistance = FVector2D::Distance(AirplaneJumpPosXY, LandingTargetPosXY);
	
	// 극단적으로 비행기 경로에서 LandingDestination이 가까운 상황 (XY평면 기준)
	if (JumpPosToLandingPosDistance < 1.f)
	{
		UC_Util::Print("From SetSkyDivingDestination : Very short travel distance!", FColor::MakeRandomColor(), 10.f);
		// 이 때는 XY 좌표 동일하게 가져가기
		SkyDivingComponent->SetSkyDivingStateDestination({LandingTargetPosXY.X, LandingTargetPosXY.Y, ParachuteDeployLimitHeight});
		return;
	}
	
	//  LandingTargetPos(XY평면 기준) 까지의 거리의 80% 까지 도달하도록 설정
	FVector2D Direction = (LandingTargetPosXY - AirplaneJumpPosXY).GetSafeNormal();
	FVector2D DestinationXY = AirplaneJumpPosXY + Direction * JumpPosToLandingPosDistance * 0.8f;
	SkyDivingComponent->SetSkyDivingStateDestination({DestinationXY.X, DestinationXY.Y, ParachuteDeployLimitHeight});

	// TODO : 밑에 라인 지우기 (For testing)
	GAMESCENE_MANAGER->GetPlayer()->GetMainMapWidget()->SpawnSkyDivingStateDestinationImage({DestinationXY.X, DestinationXY.Y, ParachuteDeployLimitHeight});
}

FVector2D UC_BTTaskSkyDive::GetClosestPointOnLine(const FVector2D& TargetPoint, const FVector2D& LineStart, const FVector2D& LineEnd) const
{
	FVector2D LineStartToEnd = LineEnd - LineStart;
	FVector2D LineStartToTargetPoint = TargetPoint - LineStart;

	FVector2D LineDirection = LineStartToEnd.GetSafeNormal();
	float T = FVector2D::DotProduct(LineStartToTargetPoint, LineDirection); // 투영된 스칼라 값

	T = FMath::Clamp(T, 0.0f, LineStartToEnd.Size());
	return LineStart + T * LineDirection;
}
