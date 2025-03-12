// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/SkyDiveTask/C_BTTaskSkyDive.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Singleton/C_GameSceneManager.h"

#include "Character/C_Enemy.h"
#include "Utility/C_Util.h"

UC_BTTaskSkyDive::UC_BTTaskSkyDive()
{
}

void UC_BTTaskSkyDive::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
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
	}
	
	// SkyDiving 도착지점 세팅
	SetRandomLandingTargetLocation();

	// TODO : 비행기에서 뛰어내릴 지점 정해야 함 -> 가장 직선거리가 짧은 거리 or 뛰어내려도 충분히 도달할 거리로 Random하게

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
