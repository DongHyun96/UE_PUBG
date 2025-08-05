// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_JumpTrainingTable.generated.h"


UCLASS()
class UE_PUBG_API AC_JumpTrainingTable : public AActor
{
	GENERATED_BODY()

public:
	AC_JumpTrainingTable();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void OnBoxColliderBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

	UFUNCTION()
	void OnBoxColliderEndOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex
	);

private:
	
	/// <summary>
	/// Delegate 구독을 통해 Player Number 1 Key input action 처리용 static 함수 
	/// </summary>
	/// <returns> : TrainingGround내에 배치된 JumpTrainingTable 중 하나라도 Focused 처리된 상황이라면 return true </returns>
	static bool OnNumber1KeyInteraction();

	/// <summary>
	/// <para> Delegate 구독을 통해 Player Number 2 Key input action 처리용 static 함수 </para>
	/// <para> TrainingGround내의 SkyDivingTester SkyDiving 처리 </para>
	/// </summary>
	/// <returns> : IsFocused 처리되지 않은 JumpTrainingTable일 경우 return false </returns>
	bool OnNumber2KeyInteraction();

private:

	UShapeComponent* BoxColliderComponent{};
	UStaticMeshComponent* StaticMeshComponent{};

	TArray<UPrimitiveComponent*> PrimitiveComponents{};

private:

	// SkyDiving 시작 지점
	USceneComponent* SkyDivingStartLocation{};

private:

	// Training Ground에 배치된 모든 JumpTrainingTable들
	static TArray<AC_JumpTrainingTable*> JumpTrainingTables;

	// 현재 이 테이블이 focused되어있는지
	bool bIsFocused{};

private:
	
	class UWidgetComponent* TriangleWidgetComponent{};
	UUserWidget* TriangleWidget{};

	UUserWidget* InteractionInfoWidget{};
	float InteractionInfoWidgetOpacityDestination{};

	
};


