// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "C_BTTaskSkyDive.generated.h"

/**
 * 
 */


/*
TODO :
SkyDive할 목표지점 설정(땅)
최단 거리가 되었을 때 비행기에서 내리기
비행기에서 내린 이후 목표 지점으로 SkyDive 진행
*/

UCLASS()
class UE_PUBG_API UC_BTTaskSkyDive : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UC_BTTaskSkyDive();

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	
	/// <summary>
	/// SKYDIVE_TARGET_TILES의 영역 내에서 Random한 위치로 TargetLocation 세팅
	/// </summary>
	/// <return> 제대로 setting 되지 않았다면 return false </return>
	bool SetRandomLandingTargetLocation();

private:
	
	class AC_Enemy*				OwnerEnemy{};
	class UC_BehaviorComponent* OwnerBehaviorComponent{};

private:

	// Shanty-town에서 적절한 Landing 위치의 Tile 좌표들 (100 * 100 타일 크기 기준)
	const TArray<TPair<uint8, uint8>> SKYDIVE_LANDING_TARGET_TILES =
	{
		{2, 5}, {2, 6}, {2, 7},
		{3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8},
		{4, 4}, {4, 5},
		{5, 4}, {5, 5},
		{6, 5}
	};
};


