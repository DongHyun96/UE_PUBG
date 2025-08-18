// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_InteractableTable.h"
#include "C_CombatControlTable.generated.h"

UCLASS()
class UE_PUBG_API AC_CombatControlTable : public AC_InteractableTable
{
	GENERATED_BODY()

public:
	AC_CombatControlTable();

protected:
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

private:
	
	/// <summary>
	/// F Key Interaction 처리 (Toggle Start/Stop Simulating Enemy AI fight) 
	/// </summary>
	/// <returns> : 이 Table이 Focused되어 있고, 제대로 Interaction 처리되었다면 return true </returns>
	bool OnFKeyInteraction();

	void OnBoxColliderBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	) override;

	void OnBoxColliderEndOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex
	) override;

private:

	class UC_CombatFieldWidget* CombatFieldWidget{};

};
