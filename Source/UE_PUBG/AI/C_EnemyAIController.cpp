// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_EnemyAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/C_Player.h"
#include "Character/C_Enemy.h"
#include "AI/C_BehaviorComponent.h"
#include "Singleton/C_GameSceneManager.h"

#include "Utility/C_Util.h"


const TMap<ESightRangeLevel, float> AC_EnemyAIController::SIGHT_RANGE_DISTANCE =
{
	{ESightRangeLevel::Level1, 20.f},
	{ESightRangeLevel::Level2, 50.f},
	{ESightRangeLevel::Level3, 150.f},
	{ESightRangeLevel::Level4, 250.f},
};


AC_EnemyAIController::AC_EnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	Blackboard			= CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception"); // Sight에 들어왔을 때의 행동 정의
	Sight				= CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");		// 시야 | Perception, sight 둘이 세트로 씀

	Sight->SightRadius								= 600.f;	// 6m
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

	// TODO : 이 라인 지우기
	BehaviorComponent->SetPlayer(GAMESCENE_MANAGER->GetPlayer());
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
	BehaviorComponent->SetOwnerEnemy(OwnerCharacter);
	
	SetGenericTeamId(OwnerCharacter->GetGenericTeamId());

	// PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AC_EnemyAIController::OnPerceptionUpdated);
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AC_EnemyAIController::OnTargetPerceptionUpdated);
	
	// Set Black board & Behavior tree

	UBlackboardComponent* blackBoard = Blackboard;

	UseBlackboard(OwnerCharacter->GetBehaviorTree()->GetBlackboardAsset(), blackBoard);

	BehaviorComponent->SetBlackboard(blackBoard);

	RunBehaviorTree(OwnerCharacter->GetBehaviorTree());
}

/*void AC_EnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	// Not in used
	/*for (AActor* actor : UpdatedActors)
	{
		// BehaviorComponent->SetTargetLocation(actor->GetActorLocation());
		// if (AC_Player* Player = Cast<AC_Player>(actor))
		// {
		// 	BehaviorComponent->SetTargetLocation(Player->GetActorLocation());
		// 	Blackboard->SetValueAsObject("Player", Player);
		// 	return;
		// }
	}
}*/

void AC_EnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus)
{
	if (Stimulus.Type != UAISense::GetSenseID<UAISense_Sight>()) return; // 시야 Sense가 아니라면 return

	AC_BasicCharacter* PerceptionUpdatedCharacter = Cast<AC_BasicCharacter>(Actor);
	if (!IsValid(PerceptionUpdatedCharacter)) return; // 인지된 Actor가 Character가 아닐 때
	
	if (Stimulus.WasSuccessfullySensed()) // 새로 인지된 Actor인 상황
	{
		// BehaviorComponent->SetTargetCharacter(Actor);

		// 거리를 측정해서 DetectedCharacters에 추가
		RemoveCharacterFromDetectedCharacters(PerceptionUpdatedCharacter); // 추가하기 이전 duplicated 상황 사전 방지
		AddCharacterToDetectedCharacters(PerceptionUpdatedCharacter);
		return;
	}

	// Lose sight된 상황, Check distance
	if (FVector::Distance(OwnerCharacter->GetActorLocation(), Actor->GetActorLocation()) > SIGHT_RANGE_DISTANCE[ESightRangeLevel::Level4])
	{
		// Sight Range Level 최대거리에서 벗어난 캐릭터일 경우
		if (BehaviorComponent->GetTargetCharacter() == Actor)
			BehaviorComponent->SetTargetCharacter(nullptr);

		RemoveCharacterFromDetectedCharacters(PerceptionUpdatedCharacter);	
	}
}

void AC_EnemyAIController::UpdateDetectedCharactersRangeLevel()
{
	// TODO : DetectedCharacters에 들어있는 Character들 모두 RangeLevel 갱신시키기
	
}

bool AC_EnemyAIController::RemoveCharacterFromDetectedCharacters(class AC_BasicCharacter* TargetCharacter)
{
	bool Removed{};
	
	for (int i = 0; i < static_cast<int>(ESightRangeLevel::Max); ++i)
	{
		ESightRangeLevel Level = static_cast<ESightRangeLevel>(i);
		Removed = Removed || DetectedCharacters[Level].Remove(TargetCharacter);
	}

	return Removed;
}

bool AC_EnemyAIController::AddCharacterToDetectedCharacters(class AC_BasicCharacter* InCharacter)
{
	float DistanceBetweenTwoCharacters = FVector::Distance(OwnerCharacter->GetActorLocation(), InCharacter->GetActorLocation());
	
	for (int i = 0; i < static_cast<int>(ESightRangeLevel::Max); ++i)
	{
		ESightRangeLevel Level = static_cast<ESightRangeLevel>(i);
		float Distance = SIGHT_RANGE_DISTANCE[Level];

		if (DistanceBetweenTwoCharacters < Distance)
		{
			DetectedCharacters[Level].Add(InCharacter);
			return true;
		}
	}
	return false;
}

