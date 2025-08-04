// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Task/IdleTask/C_BTTaskStatCare.h"
#include "C_BTTaskStatCareTestStatCare.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_BTTaskStatCareTestStatCare : public UC_BTTaskStatCare
{
	GENERATED_BODY()

private:

	bool TryUsingHealItem(AC_Enemy* Enemy, UC_InvenComponent* EnemyInvenComponent) override;

	bool TryUsingBoostItem(AC_Enemy* Enemy, UC_InvenComponent* EnemyInvenComponent) override;

private:
	
	/// <summary>
	/// MedKit 사용 시도 
	/// </summary>
	/// <returns> : 사용할 수 없다면 return false </returns>
	bool TryUseMedKit(AC_Enemy* Enemy, UC_InvenComponent* EnemyInvenComponent);
};
