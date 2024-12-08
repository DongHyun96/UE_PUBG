// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ConsumableItem/C_HealItem.h"
#include "C_FirstAidKit.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API AC_FirstAidKit : public AC_HealItem
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

	//bool Interaction(AC_BasicCharacter* Character) override;

private:

	void HandleActivatingState()	override;
	void OnStartUsing()				override;
	void OnActivatingFinish()		override;

	void OnCancelActivating()		override;
};
