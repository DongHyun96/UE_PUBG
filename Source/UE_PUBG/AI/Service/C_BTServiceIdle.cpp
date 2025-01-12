// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/C_BTServiceIdle.h"

#include "Singleton/C_GameSceneManager.h"

#include "Character/C_Player.h"
#include "AI/C_EnemyAIController.h"
#include "AI/C_BehaviorComponent.h"

UC_BTServiceIdle::UC_BTServiceIdle()
{
	NodeName = "Idle";
}

void UC_BTServiceIdle::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AC_EnemyAIController* Controller = Cast<AC_EnemyAIController>(OwnerComp.GetOwner());
	UC_BehaviorComponent* BehaviorComponent = Controller->GetBehaviorComponent();

	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();

	if (IsValid(Player)) BehaviorComponent->SetPlayer(Player);

	BehaviorComponent->ChangeServiceType(EServiceType::IDLE);

}
