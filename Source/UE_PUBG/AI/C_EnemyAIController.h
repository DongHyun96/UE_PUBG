// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "C_EnemyAIController.generated.h"

/// <summary>
/// Sight에 잡혔던 Character들의 처리를 거리별로 나누어 처리 (ex. 거리로만 따졌을 때, TargetCharacter로 잡힐 Character는 Level1의 Character가 가장 높은 우선순위를 가짐)
/// </summary>
enum class ESightRangeLevel : uint8
{
	Level1, // 20m 이내
	Level2, // 50m 이내
	Level3, // 150m 이내
	Level4, // 250m 이내
	Max
};

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_EnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AC_EnemyAIController();

	void BeginPlay()				override;
	void Tick(float DeltaTime)		override;

	void OnPossess(APawn* InPawn)	override;

	/// <summary>
	/// Sight Radius안에 들어오면 발동할 함수
	/// </summary>
	/// <param name="UpdatedActors"> : 인지한 Actors 배열</param>
	/*UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);*/

	/// <summary>
	/// Sight에 처음으로 잡히거나 Lose sight 되었을 때 호출될 함수 
	/// </summary>
	/// /// <param name="Actor"> : Perception Updated된 Actor </param>
	/// /// <param name="Stimulus"></param>
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	class UC_BehaviorComponent* GetBehaviorComponent() const { return BehaviorComponent; }

public:

	/// <summary>
	/// DetectedCharacters 내에 존재하는 Character들의 RangeLevel 갱신시키기
	/// </summary>
	void UpdateDetectedCharactersRangeLevel();
	
private:
	
	/// <summary>
	/// DetectedCharacters에서 해당 캐릭터 제거 (평균 O(1) 연산)
	/// </summary>
	/// <returns> : 해당 Character가 존재하지 않았다면 return false </returns>
	bool RemoveCharacterFromDetectedCharacters(class AC_BasicCharacter* TargetCharacter);

	/// <summary>
	/// Sight Range Level을 따져서 DetectedCharacters에 Character 추가
	/// </summary>
	/// <param name="InCharacter"></param>
	/// <returns> : 추가할 수 없는 거리라면 return false </returns>
	bool AddCharacterToDetectedCharacters(class AC_BasicCharacter* InCharacter);

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UAISenseConfig_Sight* Sight{};

	class AC_Enemy* OwnerCharacter{};

	/*UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float BehaviorRange = 150.f;*/

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UC_BehaviorComponent* BehaviorComponent{};

private:

	// Sight Range Level별 거리
	static const TMap<ESightRangeLevel, float> SIGHT_RANGE_DISTANCE;

	// Sight에 한 번 잡혔었고, SightRangeLevel 최대에서 아직 벗어나지 않는 캐릭터들
	TMap<ESightRangeLevel, TSet<class AC_BasicCharacter*>> DetectedCharacters =
	{
		{ESightRangeLevel::Level1, {}},
		{ESightRangeLevel::Level2, {}},
		{ESightRangeLevel::Level3, {}},
		{ESightRangeLevel::Level4, {}}
	};
	
};

