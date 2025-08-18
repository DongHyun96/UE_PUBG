// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_InteractableTable.h"
#include "GameFramework/Actor.h"
#include "C_JumpTrainingTable.generated.h"


UCLASS()
class UE_PUBG_API AC_JumpTrainingTable : public AC_InteractableTable
{
	GENERATED_BODY()

public:
	AC_JumpTrainingTable();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	
	/// <summary>
	/// Delegate 구독을 통해 Player Number 1 Key input action 처리용 함수 
	/// </summary>
	/// <returns> : 이 Table이 Focused 처리된 상황이라면 Num1 key interaction 이 후, return true </returns>
	bool OnNumber1KeyInteraction();

	/// <summary>
	/// <para> Delegate 구독을 통해 Player Number 2 Key input action 처리용 함수 </para>
	/// <para> TrainingGround내의 SkyDivingTester SkyDiving 처리 </para>
	/// </summary>
	/// <returns> : IsFocused 처리되지 않은 JumpTrainingTable일 경우 return false </returns>
	bool OnNumber2KeyInteraction();

private:

	// SkyDiving 시작 지점
	USceneComponent* SkyDivingStartLocation{};

	UUserWidget* InteractionInfoWidget{};
	
};


