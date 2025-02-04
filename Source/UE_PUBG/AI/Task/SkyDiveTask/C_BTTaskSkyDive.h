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
SkyDive�� ��ǥ���� ����(��)
�ִ� �Ÿ��� �Ǿ��� �� ����⿡�� ������
����⿡�� ���� ���� ��ǥ �������� SkyDive ����
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
	
	class AC_Enemy* OwnerEnemy{};
};
