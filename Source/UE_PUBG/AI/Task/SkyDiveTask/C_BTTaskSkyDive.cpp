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

	// SkyDiving이 잘 끝났다고 간주 IdleTask로 돌아가기
	if (OwnerEnemy->GetMainState() != EMainState::SKYDIVING)
	{
		OwnerBehaviorComponent->SetServiceType(EServiceType::IDLE);
		OwnerBehaviorComponent->SetIdleTaskType(EIdleTaskType::WAIT);	

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		
		UC_Util::Print
		(
			"From BTTaskSkyDive TickTask : Task completed due to Character MainState set to different State",
			FColor::MakeRandomColor(), 10.f
		);
		return;
	}

	switch (OwnerSkyDivingComponent->GetSkyDivingState())
	{
	case ESkyDivingState::READY:
	{
		// Jump 위치 확인해서 도달했다면 Jump 하기
		FVector AirplaneLocation = GAMESCENE_MANAGER->GetAirplaneManager()->GetAirplane()->GetActorLocation();
		FVector2D AirplanePosXY = { AirplaneLocation.X, AirplaneLocation.Y };

		float DistanceToJumpLocation = FVector2D::Distance(AirplanePosXY, AirplaneJumpPosXY); 
		
		if (DistanceToJumpLocation < 500.f || bSkyDiveStartFlag)
		{
			bSkyDiveStartFlag = true; // SKyDiving할 수 없는 상황일 수도 있기 때문에 다음 Tick에서 SkyDiving 다시 시도하는 장치 마련
			OwnerSkyDivingComponent->SetSkyDivingState(ESkyDivingState::SKYDIVING);
		}
	}
		break;
	case ESkyDivingState::SKYDIVING:
		// TODO : 목적지까지 도달 처리하기
		break;
	case ESkyDivingState::PARACHUTING:
		break;
	case ESkyDivingState::LANDING:
		break;
	case ESkyDivingState::MAX:
		break;
	default:
		break;
	}
}

EBTNodeResult::Type UC_BTTaskSkyDive::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!IsValid(OwnerEnemy))
	{
		AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
		if (!IsValid(Controller))
		{
			UC_Util::Print("From UC_BTTaskSkyDive ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerEnemy = Cast<AC_Enemy>(Controller->GetPawn());

		if (!IsValid(OwnerEnemy))
		{
			UC_Util::Print("From UC_BTTaskSkyDive ExecuteTask : OwnerEnemy Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerBehaviorComponent = Controller->GetBehaviorComponent();

		if (!IsValid(OwnerBehaviorComponent))
		{
			UC_Util::Print("From UC_BTTaskSkyDive ExecuteTask : OwnerBehaviorComponent Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerSkyDivingComponent = OwnerEnemy->GetSkyDivingComponent();
	}
	
	SetRandomLandingTargetLocation();	// SkyDiving 도착지점 세팅
	SetAirplaneJumpPosXY();				// Jump할 지점 setting

	return EBTNodeResult::InProgress;
}

bool UC_BTTaskSkyDive::SetRandomLandingTargetLocation()
{
	TPair<uint8, uint8> RandomTileCoord = SKYDIVE_LANDING_TARGET_TILES[FMath::RandRange(0, SKYDIVE_LANDING_TARGET_TILES.Num() - 1)];

	FVector TileLocation = GAMESCENE_MANAGER->ConvertTileCoordinateToTileMiddleLocation(RandomTileCoord);
	float CellWorldSize  = GAMESCENE_MANAGER->GetCellWorldSize();

	float XOffset = FMath::RandRange(-CellWorldSize * 0.5f, CellWorldSize * 0.5f);
	float YOffset = FMath::RandRange(-CellWorldSize * 0.5f, CellWorldSize * 0.5f);

	// 상공에서 아래로 RayCasting해서 Hit한 지점을 Target Location으로 잡음
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(OwnerEnemy);

	FHitResult HitResult{};

	FVector StartLocation = {TileLocation.X + XOffset, TileLocation.Y + YOffset, 40000.f}; // 400m 상공에서 아래로 RayCasting
	FVector DestLocation  = StartLocation - FVector::UnitZ() * 40000.f;
	
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, DestLocation, ECollisionChannel::ECC_Visibility, CollisionParams);

	if (!HasHit)
	{
		// 거의 들어올 일 없을 예정
		
		UC_Util::Print("From UC_BTTaskSkyDive::SetRandomLandingTargetLocation : RayCasting to find Z failed! Setting alternative Z value.", FColor::Red, 10.f);
		FVector TargetLocation	= StartLocation;
		TargetLocation.Z		= 3000.f;
		OwnerBehaviorComponent->SetTargetLocation(TargetLocation);
		return false;
	}

	OwnerBehaviorComponent->SetTargetLocation(HitResult.ImpactPoint);
	return true;
}

bool UC_BTTaskSkyDive::SetAirplaneJumpPosXY()
{
	TPair<FVector, FVector> PlaneRouteStartDest = GAMESCENE_MANAGER->GetAirplaneManager()->GetPlaneRouteStartDestPair();
	FVector2D Start = {PlaneRouteStartDest.Key.X, PlaneRouteStartDest.Key.Y };
	FVector2D End	= {PlaneRouteStartDest.Value.X, PlaneRouteStartDest.Value.Y };

	FVector		DestLocation = OwnerBehaviorComponent->GetTargetLocation();
	FVector2D	Dest		 = { DestLocation.X, DestLocation.Y }; 
	
	FVector2D Dir = End - Start;
	FVector2D ToDest = Dest - Start;

	FVector2D DirNorm = Dir.GetSafeNormal(); // 단위 벡터
	float T = FVector2D::DotProduct(ToDest, DirNorm); // 투영된 스칼라 값

	// t를 0과 SegmentLength 사이로 제한
	T = FMath::Clamp(T, 0.0f, Dir.Size());

	// answer = Start + t * DirNorm;
	AirplaneJumpPosXY = Start + T * DirNorm;

	FVector JumpLocationTemp = { AirplaneJumpPosXY.X, AirplaneJumpPosXY.Y, 0.f };
	GAMESCENE_MANAGER->GetPlayer()->GetMainMapWidget()->SpawnJumpPosAndTargetPosImage(JumpLocationTemp, OwnerBehaviorComponent->GetTargetLocation());
	return true;
}
