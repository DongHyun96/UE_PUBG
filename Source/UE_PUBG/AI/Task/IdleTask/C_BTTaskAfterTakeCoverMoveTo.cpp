// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/IdleTask/C_BTTaskAfterTakeCoverMoveTo.h"

#include "C_BTTaskSmokeGrenade.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Utility/C_Util.h"

// const TMap<EPoseState, float> UC_BTTaskAfterTakeCoverMoveTo::FloorToHeadOffsets =
// {
//
// };

UC_BTTaskAfterTakeCoverMoveTo::UC_BTTaskAfterTakeCoverMoveTo()
{
	bNotifyTick = true;
}

void UC_BTTaskAfterTakeCoverMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskAfterTakeCoverMoveTo::TickTask : EnemyAIController casting failed!", FColor::Red, 10.f);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 현재 FlashBang 피격 중인 상황(현상 유지)
	if (Controller->IsFlashBangEffectTimeLeft()) return;

	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	if (Enemy->GetIsPoseTransitioning()) return;

	// End of PoseTransition
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UC_BTTaskAfterTakeCoverMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UC_Util::Print("After TakeCover MoveTo", FColor::MakeRandomColor(), 20.f);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskAfterTakeCoverMoveTo ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}
	
	AC_Enemy*			  Enemy				= Cast<AC_Enemy>(Controller->GetPawn());
	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();
	AC_BasicCharacter*	  TargetCharacter	= BehaviorComponent->GetTargetCharacter();
	
	if (!TargetCharacter) return EBTNodeResult::Succeeded; // TargetCharacter가 사라졌다 해도 TakeCover sequence 이어나가기
	if (TargetCharacter->GetMainState() == EMainState::DEAD) return EBTNodeResult::Succeeded;

	// ChangePose 할지 결정
	// 도달한 지점에서 Sight 검사를 해서 현재 Sight에 들어오는 곳이라면, 다른 자세를 취하기

	if (!Controller->IsCurrentlyOnSight(TargetCharacter)) return EBTNodeResult::Succeeded; // 제대로 Hide 처리되었다고 판단

	// 엄폐 위치에 도달했는데도 Sight에 잡힌 상황

	FVector EnemyBottomLocation		= Enemy->GetActorBottomLocation();
	FVector CrouchTestStartLocation = EnemyBottomLocation + FVector::UnitZ() * 50.f;
	FVector CrawlTestStartLocation	= EnemyBottomLocation + FVector::UnitZ() * 10.f;

	FHitResult HitResult{};
	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActor(Enemy);
	
	TArray<AActor*> AttachedActors{};
	Enemy->GetAttachedActors(AttachedActors);
	CollisionParams.AddIgnoredActors(AttachedActors);

	// Crouch 지점 trace testing
	bool HasHit = GetWorld()->LineTraceSingleByChannel
	(
		HitResult,
		CrouchTestStartLocation,
		TargetCharacter->GetActorLocation(),
		ECC_Visibility,
		CollisionParams
	);

	bool CrouchTraceHitted = HitResult.GetActor() == TargetCharacter;

	// Crawl 지점 trace testing
	HitResult = {};
	HasHit = GetWorld()->LineTraceSingleByChannel
	(
		HitResult,
		CrawlTestStartLocation,
		TargetCharacter->GetActorLocation(),
		ECC_Visibility,
		CollisionParams
	);

	bool CrawlTraceHitted = HitResult.GetActor() == TargetCharacter;

	// 만약에 불가피하게 도착지점에 아무 엄폐물도 없다하면(높이로 판단하기) TakeCover sequence 종료
	// Stat Care용 Enemy의 경우 그냥 넘어감
	if (Enemy->GetBehaviorType() != EEnemyBehaviorType::StatCareTest && CrouchTraceHitted && CrawlTraceHitted)
	{
		UC_Util::Print("From AfterTakeCover : TakeCover sequence failed!, Returning to wait task", FColor::Red, 20.f);
		BehaviorComponent->SetIdleTaskType(EIdleTaskType::WAIT);
		return EBTNodeResult::Failed;
	}

	// 자세 전환 처리
	EPoseState ChangeTo = !CrouchTraceHitted ? EPoseState::CROUCH : EPoseState::CRAWL;

	if (Enemy->GetPoseState() == ChangeTo) return EBTNodeResult::Succeeded;
	
	Enemy->SetPoseState(Enemy->GetPoseState(), EPoseState::CROUCH);
	
	return EBTNodeResult::InProgress;
}
