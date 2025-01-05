// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_EnemyAIController.h"

AC_EnemyAIController::AC_EnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	//Blackboard			= CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
	//PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception"); // Sight에 들어왔을 때의 행동 정의
	//Sight				= CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");		// 시야 | Perception, sight 둘이 세트로 씀

	//Sight->SightRadius								= 600.f;
	//Sight->LoseSightRadius							= 800.f;
	//Sight->PeripheralVisionAngleDegrees				= 90.f;
	//Sight->SetMaxAge(2);											// LoseSight를 넘어갔을 때 2초 있다가 놓침
	//
	//// Affiliation - 소속, 그룹 | 그룹을 설정해서 사용가능
	//Sight->DetectionByAffiliation.bDetectFriendlies = false;		
	//Sight->DetectionByAffiliation.bDetectEnemies	= true;
	//Sight->DetectionByAffiliation.bDetectNeutrals	= false;

	//PerceptionComponent->ConfigureSense(*Sight);
	//PerceptionComponent->SetDominantSense(Sight->GetSenseImplementation()); 

	//// Behavior Component
	//BehaviorComponent = CreateDefaultSubobject<UC_BehaviorComponent>("BehaviorComponent");
}

void AC_EnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AC_EnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_EnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//OwnerCharacter = Cast<AC_Enemy>(InPawn);

	//SetGenericTeamId(OwnerCharacter->GetGenericTeamId());

	//PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AC_EnemyAIController::OnPerceptionUpdated);

	//// Set Black board & Behavior tree

	//UBlackboardComponent* blackBoard = Blackboard;

	//UseBlackboard(OwnerCharacter->GetBehaviorTree()->GetBlackboardAsset(), blackBoard);

	//BehaviorComponent->SetBlackboard(blackBoard);

	//RunBehaviorTree(OwnerCharacter->GetBehaviorTree());
}

void AC_EnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{

}
