// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ConsumableItem/C_ConsumableItem.h"
#include "C_FirstAidKit.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_FirstAidKit : public AC_ConsumableItem
{
	GENERATED_BODY()

public:
	AC_FirstAidKit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	bool StartUsingConsumableItem(class AC_BasicCharacter* InItemUser) override;
	void HandleActivatingState() override;
	void HandleActivateCompletedState() override; // Hook Method

private:
	
	//0.24초씩 -> +15 : 총 5번 해서 총 2초에 최대 75만큼 힐
	const float ONE_BLOCK_HEAL_AMOUNT	= 15.f;
	const float ONE_BLOCK_TIME			= 0.24f;
	const UINT	TOTAL_USING_BLOCK_CNT	= 5;

private:

	// 총 이용 Block
	UINT BlockUsed{};

};
