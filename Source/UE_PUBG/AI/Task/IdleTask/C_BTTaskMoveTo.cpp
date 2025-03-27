// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/IdleTask/C_BTTaskMoveTo.h"

#include "NavigationSystem.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utility/C_Util.h"

UC_BTTaskMoveTo::UC_BTTaskMoveTo()
{
	bNotifyTick = true;
}

void UC_BTTaskMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UC_BTTaskMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// First initialization
	if (!IsValid(OwnerEnemy))
	{
		OwnerAIController = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
		if (!IsValid(OwnerAIController))
		{
			UC_Util::Print("From UC_BTTaskMoveTo ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		OwnerEnemy = Cast<AC_Enemy>(OwnerAIController->GetPawn());

		if (!IsValid(OwnerEnemy))
		{
			UC_Util::Print("From UC_BTTaskMoveTo ExecuteTask : OwnerEnemy Casting failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}
		
		NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

		if (!IsValid(NavSystem))
		{
			UC_Util::Print("From UC_BTTaskMoveTo ExecuteTask : NavSystem not inited!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}
	}
	
	// 만약 피치 못해 NavMesh가 깔려 있지 않은 지역에 도달했다면 가장 근처의 NavMesh로 귀환 -> 어쩔 수 없다고 칠까 그냥...
	/*if (!IsAgentOnNavMesh() && !bHasMoveToNearestNavMeshStarted)
	{
		// AI MoveTo로 이동시키기
		// OwnerAIController->MoveTo
		
		// 1m 이내의 가장 가까운 NavMesh 위치 찾기
		FNavLocation NavMeshLocation{};
		bool bNavMeshFounded = NavSystem->ProjectPointToNavigation(OwnerEnemy->GetActorLocation(), NavMeshLocation, FVector(500.f, 500.f, 500.f));
		if (!bNavMeshFounded)
		{
			UC_Util::Print("From UC_BTTaskMoveTo : Nearest Nav Mesh not found on 5m extent!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}

		EPathFollowingRequestResult::Type RequestType = OwnerAIController->MoveToLocation(NavMeshLocation.Location);
		if (RequestType == EPathFollowingRequestResult::Type::Failed)
		{
			UC_Util::Print("From UC_BTTaskMoveTo : MoveTo nearest navmesh Request failed!", FColor::Red, 10.f);
			return EBTNodeResult::Failed;
		}
		
		bHasMoveToNearestNavMeshStarted = true;
	}
	
	else bHasMoveToNearestNavMeshStarted = false;

	UC_Util::Print("MoveExecute", FColor::Red);*/
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

bool UC_BTTaskMoveTo::IsAgentOnNavMesh()
{
	static const float ACTOR_HALF_Z = 90.f;
	FVector ActorBottomLocation = OwnerEnemy->GetActorLocation() - FVector::UnitZ() * ACTOR_HALF_Z;
	FNavLocation OutLocation{};
	return NavSystem->ProjectPointToNavigation(ActorBottomLocation, OutLocation, FVector(10.f, 10.f, 10.f));
}
