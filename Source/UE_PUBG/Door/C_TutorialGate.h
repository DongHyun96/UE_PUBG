// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_TutorialGate.generated.h"

UCLASS()
class UE_PUBG_API AC_TutorialGate : public AActor
{
	GENERATED_BODY()

public:
	AC_TutorialGate();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UShapeComponent* GetGateOpeningBoxComponent() const { return GateOpeningBox; }

public:

	UFUNCTION(BlueprintImplementableEvent)
	void CloseGate();

	/// <summary>
	/// Opening Trigger Box 활성화 / 비활성화
	/// </summary>
	void ToggleOpeningBoxTriggerEnabled(bool Enabled);

	UFUNCTION(BlueprintImplementableEvent)
	void OpenGate();

private:

	UFUNCTION()
	void OnGateOpeningBoxBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

	/// <summary>
	/// <para> Tutorial에서 사용되는 Gate에서는 사용되지 않을 예정 </para>
	/// <para> Player vs Enemy Field Gate에서 사용될 함수 </para>
	/// </summary>
	UFUNCTION()
	void OnGateOpeningBoxEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor*				 OtherActor,
		UPrimitiveComponent* OtherComp,
		int32				 OtherBodyIndex
	);



private:

	UShapeComponent* GateOpeningBox{};
	
	class UWidgetComponent* TriangleWidgetComponent{};

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bOpenOnStart{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUseOpeningBoxEndOverlapToCloseGate{};	
	
};
