// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/C_EnemyAIController.h"

#include "ParticleHelper.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/C_Player.h"
#include "Character/C_Enemy.h"
#include "AI/C_BehaviorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "ProfilingDebugging/StallDetector.h"
#include "Service/C_BTServiceCombat.h"
#include "Singleton/C_GameSceneManager.h"

#include "Utility/C_Util.h"

const TMap<ESightRangeLevel, float> AC_EnemyAIController::SIGHT_RANGE_DISTANCE =
{
	{ESightRangeLevel::Level1, 200.f},
	{ESightRangeLevel::Level2, 500.f},
	{ESightRangeLevel::Level3, 1500.f},
	{ESightRangeLevel::Level4, 2500.f},
};


AC_EnemyAIController::AC_EnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	Blackboard			= CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception"); // Sight에 들어왔을 때의 행동 정의
	Sight				= CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");		// 시야 | Perception, sight 둘이 세트로 씀

	Sight->SightRadius								= SIGHT_RANGE_DISTANCE[ESightRangeLevel::Level4];
	Sight->LoseSightRadius							= Sight->SightRadius + 100.f;
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

	DrawSightRange();

	// Update FlashBangEffectLeftTime
	FlashBangEffectLeftTime = FMath::Max(FlashBangEffectLeftTime - DeltaTime, 0.f);
	OwnerCharacter->SetStunnedTime(FlashBangEffectLeftTime);
}

void AC_EnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UC_Util::Print("Enemy OnPossess", FColor::Red, 10.f);

	OwnerCharacter = Cast<AC_Enemy>(InPawn);
	BehaviorComponent->SetOwnerEnemy(OwnerCharacter);
	BehaviorComponent->SetOwnerEnemyAIController(this);
	
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

void AC_EnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.Code != EPathFollowingResult::Success) return;

	// Take Cover sequence 처리 중인 상황
	if (BehaviorComponent->GetServiceType() == EServiceType::IDLE && BehaviorComponent->GetIdleTaskType() == EIdleTaskType::TAKE_COVER)
		return;

	// Success 처리 되었고, TakeCover sequence 중이 아니라면 Wait 상태로 돌아가기
	BehaviorComponent->SetIdleTaskType(EIdleTaskType::WAIT);
}

float AC_EnemyAIController::GetLv1SightRangeDistance()
{
	// TODO : 밑 줄 주석으로 사용 / 현재는 Testing용 값 return 중
	return 2000.f;
	// return SIGHT_RANGE_DISTANCE[ESightRangeLevel::Level1];
}

bool AC_EnemyAIController::TrySetTargetCharacterToLevel1EnteredCharacter()
{
	if (DetectedCharacters[ESightRangeLevel::Level1].IsEmpty()) return false;

	// 시야에 포착되는 캐릭터 중 거리가 가장 가까운 캐릭터로 세팅 시도
	// 시야에 포착되는 캐릭터가 없다면 거리가 가장 가까운 캐릭터로 세팅하기
	TArray<AC_BasicCharacter*> Level1Characters = DetectedCharacters[ESightRangeLevel::Level1].Array();

	FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	
	Level1Characters.Sort([OwnerLocation](const AC_BasicCharacter& A, const AC_BasicCharacter& B)
	{
		// 오름차순 정렬
		return FVector::DistSquared(A.GetActorLocation(), OwnerLocation) <
			FVector::DistSquared(B.GetActorLocation(), OwnerLocation);  
	});

	for (AC_BasicCharacter* Level1Character : Level1Characters)
		if (IsCurrentlyOnSight(Level1Character))
			return BehaviorComponent->SetTargetCharacter(Level1Character);

	// 시야에 포착되는 캐릭터가 없다면 거리가 가장 가까운 캐릭터로 세팅
	return BehaviorComponent->SetTargetCharacter(Level1Characters[0]);
}

bool AC_EnemyAIController::TrySetTargetCharacterBasedOnPriority()
{
	// 이미 누군가를 공격 중이라면
	if (BehaviorComponent->GetServiceType() == EServiceType::COMBAT) return false;

	// Level 1부터 4까지 차례로 우선순위대로 TargetCharacter setting 시도
	for (int i = 0; i < static_cast<int>(ESightRangeLevel::Max); ++i)
	{
		ESightRangeLevel Level = static_cast<ESightRangeLevel>(i);
		if (DetectedCharacters[Level].IsEmpty()) continue;

		int RandomIndex = FMath::RandRange(0, DetectedCharacters[Level].Num() - 1), CurrentIndex{};
		AC_BasicCharacter* PickedCharacter{};
		for (AC_BasicCharacter* Lv1Character : DetectedCharacters[Level])
		{
			if (CurrentIndex == RandomIndex)
			{
				PickedCharacter = Lv1Character;
				break;
			}
			++CurrentIndex;
		}
		if (BehaviorComponent->SetTargetCharacter(PickedCharacter)) return true; 
	}
	return false;
}

void AC_EnemyAIController::UpdateDetectedCharactersRangeLevel()
{
	// DetectedCharacters에 들어있는 Character들 모두 RangeLevel 갱신시키기
	
	TSet<AC_BasicCharacter*> Updated{}; // 이미 갱신 처리된 Character들

	for (int i = 0; i < static_cast<int>(ESightRangeLevel::Max); i++)
	{
		ESightRangeLevel CurrentLevel = static_cast<ESightRangeLevel>(i);

		for (TSet<AC_BasicCharacter*>::TIterator It(DetectedCharacters[CurrentLevel]); It; ++It)
		{
			AC_BasicCharacter* CurrentCharacter = *It;
			if (Updated.Contains(CurrentCharacter)) continue; // 이미 조사한 캐릭터에 대해 넘어감
			
			Updated.Add(CurrentCharacter);

			// 새로운 거리로 갱신 시키기
			It.RemoveCurrent();
			
			// 이미 죽은 Character이거나 삭제된 캐릭터라면 빼버리고 진행
			if (!IsValid(CurrentCharacter))								continue;
			if (CurrentCharacter->GetMainState() == EMainState::DEAD)	continue; 
				
			AddCharacterToDetectedCharacters(CurrentCharacter);
		}
	}

	// TODO : 이 라인 지우기
	for (TPair<ESightRangeLevel, TSet<AC_BasicCharacter*>>& Pair : DetectedCharacters)
	{
		FString Str =	(Pair.Key == ESightRangeLevel::Level1) ? "Lv1 : " :
						(Pair.Key == ESightRangeLevel::Level2) ? "Lv2 : " :
						(Pair.Key == ESightRangeLevel::Level3) ? "Lv3 : " : "Lv4 : ";

		for (AC_BasicCharacter* CurrentCharacter : Pair.Value)
		{
			Str += CurrentCharacter->GetName();
			Str += "  ";
		}

		OwnerCharacter->SetSightRangeCharactersName(Pair.Key, Str);
	}
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

void AC_EnemyAIController::DrawSightRange()
{
	if (OwnerCharacter->GetMainState() == EMainState::DEAD) return;
	
	FVector Center = OwnerCharacter->GetActorLocation();

	for (int i = 0; i < static_cast<int>(ESightRangeLevel::Max); ++i)
	{
		ESightRangeLevel Level = static_cast<ESightRangeLevel>(i);
		
		DrawDebugCircle
		(
			GetWorld(),
			Center,
			SIGHT_RANGE_DISTANCE[Level],
			300,
			(i % 2 == 0) ? FColor::Green : FColor::Blue,
			false,
			-1.f, 0, 0,
			OwnerCharacter->GetActorRightVector(),
			OwnerCharacter->GetActorForwardVector()
		);
		
		Center.Z += 1.f;
	}

	// Lose Sight Circle
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
}

bool AC_EnemyAIController::IsCurrentlyOnSight(AC_BasicCharacter* TargetCharacter)
{
	if (const FActorPerceptionInfo* ActorPerceptionInfo = PerceptionComponent->GetActorInfo(*TargetCharacter))
		for (FAIStimulus Stimulus : ActorPerceptionInfo->LastSensedStimuli)
			if (Stimulus.WasSuccessfullySensed()) return true;
	
	return false;
}
