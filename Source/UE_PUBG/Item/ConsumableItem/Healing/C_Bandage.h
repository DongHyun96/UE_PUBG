// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ConsumableItem/C_HealItem.h"
#include "C_Bandage.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_Bandage : public AC_HealItem
{
	GENERATED_BODY()

public:
	AC_Bandage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//bool Interaction(AC_BasicCharacter* Character) override;

private:

	void HandleActivatingState()	override;
	void OnStartUsing()				override;
	void OnActivatingFinish()		override;
	void OnCancelActivating()		override;

	void HandleDestroy()			override;

private:

	void HideUsageMesh();

private:

	struct FTimerHandle TimerHandle{};
	static const float USAGE_MESH_SHOWN_TIME;

private:

	// 붕대 나머지 한개 까지 모두 소모했을 때 Destroy 하는 Flag, True면 Destroy
	bool bDestroyFlag{};

};
