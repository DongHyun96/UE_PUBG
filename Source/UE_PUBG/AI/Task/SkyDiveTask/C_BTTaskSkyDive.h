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
	/// <para> SKYDIVE_TARGET_TILES의 영역 내에서 Random한 위치로 TargetLocation 세팅 -> </para>
	/// <para> 비행기에서 SkyDive 시작할 지점 정하기 (낙하 목표위치와 가장 가까운 지점으로 setting) </para>
	/// </summary>
	/// <return> 제대로 setting 되지 않았다면 return false </return>
	bool SetRandomLandingTargetLocationAndJumpPosXY(class AC_Enemy* Enemy);

	/// <summary>
	/// LandingTargetLocation에 살짝 못 미치는 거리로 낙하산을 펼치기 전까지의 목표 지점 setting
	/// </summary>
	void SetSkyDivingStateDestination(class AC_Enemy* Enemy);

private:
	
	/// <summary>
	/// 특정 위치로부터 Line에 수선의 발을 내린 Line에서의 최단거리 지점 Get
	/// </summary>
	/// <param name="TargetPoint"> : 특정 위치 </param>
	/// <param name="LineStart"> : Line 시작점 </param>
	/// <param name="LineEnd"> : Line 끝점 </param>
	/// <returns></returns>
	FVector2D GetClosestPointOnLine(const FVector2D& TargetPoint, const FVector2D& LineStart, const FVector2D& LineEnd) const;


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

private:

	TMap<AC_Enemy*, FVector2D> AirplaneJumpPosXYMap{};
	TMap<AC_Enemy*, bool> SkyDiveStartFlagMap{}; // true이면 skyDive 시작해야 하는 flag
	
	// FVector2D AirplaneJumpPosXY{};
	// bool bSkyDiveStartFlag{}; // true이면 skyDive 시작해야 하는 flag
};


