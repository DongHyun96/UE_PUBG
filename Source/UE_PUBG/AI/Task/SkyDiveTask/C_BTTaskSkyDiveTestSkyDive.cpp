// Fill out your copyright notice in the Description page of Project Settings.


#include "C_BTTaskSkyDiveTestSkyDive.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "Character/Component/SkyDivingComponent/C_EnemySkyDivingComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "TrainingLevel/C_AISkyDiveTesterManager.h"
#include "TrainingLevel/C_TrainingGroundManager.h"
#include "Utility/C_Util.h"

UC_BTTaskSkyDiveTestSkyDive::UC_BTTaskSkyDiveTestSkyDive()
{
	bNotifyTick = true;
}

void UC_BTTaskSkyDiveTestSkyDive::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController* 		EnemyAIController       = Cast<AC_EnemyAIController>(OwnerComp.GetAIOwner());
	AC_Enemy*			  		Enemy                   = Cast<AC_Enemy>(EnemyAIController->GetPawn());
	UC_EnemySkyDivingComponent* EnemySkyDivingComponent = Cast<UC_EnemySkyDivingComponent>(Enemy->GetSkyDivingComponent());
	
	// SkyDiving이 잘 끝났다고 간주 IdleTask로 돌아가기 
	if (Enemy->GetMainState() != EMainState::SKYDIVING && !EnemySkyDivingComponent->IsCharacterLandingMontagePlaying())
	{
		EnemyAIController->GetBehaviorComponent()->SetServiceType(EServiceType::IDLE);
		EnemyAIController->GetBehaviorComponent()->SetIdleTaskType(EIdleTaskType::WAIT);	

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		/*UC_Util::Print
		(
			"From BTTaskSkyDive TickTask : Task completed due to Character MainState set to different State",
			FColor::MakeRandomColor(), 10.f
		);*/
	}
}

EBTNodeResult::Type UC_BTTaskSkyDiveTestSkyDive::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	if (!IsValid(Controller))
	{
		UC_Util::Print("From UC_BTTaskSkyDiveTestSkyDive ExecuteTask : Controller Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	AC_Enemy* Enemy = Cast<AC_Enemy>(Controller->GetPawn());
	if (!IsValid(Enemy))
	{
		UC_Util::Print("From UC_BTTaskSkyDiveTestSkyDive ExecuteTask : OwnerEnemy Casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	if (!GAMESCENE_MANAGER->GetTrainingGroundManager())
	{
		UC_Util::Print("From UC_BTTaskSkyDiveTestSkyDive ExecuteTask : TrainingGroundManager not valid!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	AC_AISkyDiveTesterManager* SkyDiveTesterManager = GAMESCENE_MANAGER->GetTrainingGroundManager()->GetAISkyDiveTesterManager();
	if (!SkyDiveTesterManager)
	{
		UC_Util::Print("From UC_BTTaskSkyDiveTestSkyDive ExecuteTask : SkyDiveTesterManager not valid!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}
	
	UC_EnemySkyDivingComponent* EnemySkyDivingComponent = Cast<UC_EnemySkyDivingComponent>(Enemy->GetSkyDivingComponent());
	if (!EnemySkyDivingComponent)
	{
		UC_Util::Print("From UC_BTTaskSkyDiveTestSkyDive ExecuteTask : EnemySkyDivingComponent casting failed!", FColor::Red, 10.f);
		return EBTNodeResult::Failed;
	}

	// Set Enemy Location to random SkyDiveStartLocation
	const FVector SkyDiveStartLocation = SkyDiveTesterManager->GetRandomSkyDiveStartLocation(); 
	Enemy->SetActorLocation(SkyDiveStartLocation);

	// Set Random SkyDiveLandingLocation
	const FVector LandingLocation = SkyDiveTesterManager->GetRandomSkyDiveLandingLocation();
	// Enemy->GetController<AC_EnemyAIController>()->GetBehaviorComponent()->SetBasicTargetLocation(LandingLocation);
	EnemySkyDivingComponent->SetParachutingStateDestination(LandingLocation);

	// Set SKyDiveState Destination (SkyDive 상태와 Parachuting 상태 중간의 중간도착지점 setting)
	// StartLocation으로부터 LandingLocation 까지의 80% 거리의 지점으로 setting
	FVector SkyDivingStateDestLocation = SkyDiveStartLocation + (LandingLocation - SkyDiveStartLocation) * 0.8f;
	EnemySkyDivingComponent->SetSkyDivingStateDestination(SkyDivingStateDestLocation);
	
	// Set Enemy State to SkyDive
	Enemy->SetMainState(EMainState::SKYDIVING);
	EnemySkyDivingComponent->SetSkyDivingState(ESkyDivingState::READY); // ReadState의 처리 또한 필요하기 때문에 Ready로 한 번 set했다가 바로 SkyDiving 상태로 둠
	EnemySkyDivingComponent->SetSkyDivingState(ESkyDivingState::SKYDIVING);
	
	return EBTNodeResult::InProgress;
}
