// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_EnemyAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/C_Player.h"
#include "Character/C_Enemy.h"
#include "AI/C_BehaviorComponent.h"

#include "Utility/C_Util.h"

AC_EnemyAIController::AC_EnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	Blackboard			= CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception"); // Sight에 들어왔을 때의 행동 정의
	Sight				= CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");		// 시야 | Perception, sight 둘이 세트로 씀

	Sight->SightRadius								= 600.f;
	Sight->LoseSightRadius							= 700.f;
	Sight->PeripheralVisionAngleDegrees				= 360.f;
	Sight->SetMaxAge(2);											// LoseSight를 넘어갔을 때 2초 있다가 놓침
	
	// Affiliation - 소속, 그룹 | 그룹을 설정해서 사용가능
	//Sight->DetectionByAffiliation.bDetectFriendlies = false;		
	//Sight->DetectionByAffiliation.bDetectEnemies	= true;
	//Sight->DetectionByAffiliation.bDetectNeutrals	= false;

	Sight->DetectionByAffiliation.bDetectFriendlies = true;		
	Sight->DetectionByAffiliation.bDetectEnemies	= true;
	Sight->DetectionByAffiliation.bDetectNeutrals	= true;

	PerceptionComponent->ConfigureSense(*Sight);
	PerceptionComponent->SetDominantSense(Sight->GetSenseImplementation()); 

	// Behavior Component
	BehaviorComponent = CreateDefaultSubobject<UC_BehaviorComponent>("BehaviorComponent");
}

void AC_EnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AC_EnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Center = OwnerCharacter->GetActorLocation();

	DrawDebugCircle
	(
		GetWorld(),
		Center,
		Sight->SightRadius,
		300,
		FColor::Green,
		false,
		-1.f, 0, 0,
		OwnerCharacter->GetActorRightVector(),
		OwnerCharacter->GetActorForwardVector()
	);

	TickColor = FColor::MakeRandomColor();

	Center.Z += 1.f;
	
	// Lose sight circle
	DrawDebugCircle
	(
		GetWorld(),
		Center,
		Sight->LoseSightRadius,
		300,
		FColor::Red,
		false,
		-1.f, 0, 0,
		OwnerCharacter->GetActorRightVector(),
		OwnerCharacter->GetActorForwardVector()
	);

	/*DrawDebugCircle
	(
		GetWorld(),
		Center,
		BehaviorRange,
		300,
		FColor::Red,
		false,
		-1.f, 0, 0,
		OwnerCharacter->GetActorRightVector(),
		OwnerCharacter->GetActorForwardVector()
	);*/
}

void AC_EnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UC_Util::Print("Enemy OnPossess", FColor::Red, 10.f);

	OwnerCharacter = Cast<AC_Enemy>(InPawn);

	SetGenericTeamId(OwnerCharacter->GetGenericTeamId());

	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AC_EnemyAIController::OnPerceptionUpdated);
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AC_EnemyAIController::OnTargetPerceptionUpdated);
	
	// Set Black board & Behavior tree

	UBlackboardComponent* blackBoard = Blackboard;

	UseBlackboard(OwnerCharacter->GetBehaviorTree()->GetBlackboardAsset(), blackBoard);

	BehaviorComponent->SetBlackboard(blackBoard);

	RunBehaviorTree(OwnerCharacter->GetBehaviorTree());
}

void AC_EnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	// FString str = "OnPerceptionUpdated " + FString::FromInt(UpdatedActors.Num());
	// UC_Util::Print(str, TickColor, 10.f);

	// TODO : 시야에 들어온 새로운 TargetCharacter 업데이트 로직 필요함 (or 이미 TargetCharacter인 Character에 대한 Target해제 처리)

	for (AActor* actor : UpdatedActors)
	{
		// BehaviorComponent->SetTargetLocation(actor->GetActorLocation());
		// if (AC_Player* Player = Cast<AC_Player>(actor))
		// {
		// 	BehaviorComponent->SetTargetLocation(Player->GetActorLocation());
		// 	Blackboard->SetValueAsObject("Player", Player);
		// 	return;
		// }
	}
}

void AC_EnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>()) // 시야 Sense라면,
	{
		if (Stimulus.WasSuccessfullySensed()) // 새로 인지된 Actor인 상황
		{
			
		}
		else // Lose Sight
		{
			
		}
	}
}


