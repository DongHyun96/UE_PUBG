// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/IdleTask/C_BTTaskMoveTo.h"

#include "NavigationSystem.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Utility/C_Util.h"

UC_BTTaskMoveTo::UC_BTTaskMoveTo()
{
	bNotifyTick = true;
}

void UC_BTTaskMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());

	// 현재 FlashBang 피격 중인 상황(현상 유지)
	if (Controller->IsFlashBangEffectTimeLeft()) return;
	
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UC_BTTaskMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!IsValid(NavSystem))
	{
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


	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskMoveTo::ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());

	// MoveTo 시작 시, 일어난 자세로 전환 시도(이미 일어나 있으면 상관 x)
	Enemy->SetPoseState(Enemy->GetPoseState(), EPoseState::STAND);
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

/*void UC_BTTaskMoveTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}*/

bool UC_BTTaskMoveTo::IsAgentOnNavMesh(AActor* Agent)
{
	static const float ACTOR_HALF_Z = 90.f;
	FVector ActorBottomLocation = Agent->GetActorLocation() - FVector::UnitZ() * ACTOR_HALF_Z;
	FNavLocation OutLocation{};
	return NavSystem->ProjectPointToNavigation(ActorBottomLocation, OutLocation, FVector(10.f, 10.f, 10.f));
}
