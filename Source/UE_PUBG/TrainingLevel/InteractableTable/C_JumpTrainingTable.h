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

	/// <summary>
	/// 관전 카메라 전환에 사용
	/// </summary>
	/// <returns> : IsFocused 처리되지 않은 JumpTrainingTable일 경우, 또는 Player가 SkyDiving 중이라면 return false </returns>
	bool OnFKeyInteraction();

private: // IMC_Spectator Actions (관전 카메라 중일 때, IMC_Player(Main IMC)가 빠져있기 때문에, 새로운 IMC_Spectator를 통해 binding된 아래의 함수로 input 처리)

	void OnFKey();

	void OnNum1Key();

	void OnNum2Key();
	
	/// <summary>
	/// 관전 마우스 Input callback 함수
	/// </summary>
	void OnLookInput(const struct FInputActionValue& Value);

private:

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

	// SkyDiving 시작 지점
	USceneComponent* SkyDivingStartLocation{};

	// AISkyDiveTesterManager가 들고 있는 JumpTrainingWidget
	class UC_JumpTrainingWidget* JumpTrainingWidget{};

protected: // Spectator 시점에서의 Input 처리 관련

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UInputMappingContext* IMC_Spectator{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UInputAction* FKeyAction{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UInputAction* Num1Action{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UInputAction* Num2Action{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UInputAction* LookAction{};

};


