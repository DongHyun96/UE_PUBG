// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_TutorialGoalWidget.generated.h"

UCLASS()
class UE_PUBG_API UC_TutorialGoalWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void PlayStageStartAnimation();

	/// <summary>
	/// Focused Animation 재생
	/// </summary>
	/// <param name="GoalIndex"> : Focused처리할 Animation의 Main Goal Index </param>
	/// <returns> : Index가 valid하지 않다면 return false </returns>
	bool PlayFocusedAnimation(uint8 GoalIndex);

	/// <summary>
	/// Succeeded Check Animation 재생
	/// </summary>
	/// <param name="GoalIndex"> : Focused처리할 Animation의 Main Goal Index </param>
	/// <returns> : Index가 valid하지 않다면 return false </returns>
	bool PlaySucceededAnimation(uint8 GoalIndex);

	/// <summary>
	/// 해당 GoalIndex의 FocusedAnimation이 재생중이었다면 멈추는 함수
	/// </summary>
	/// <param name="GoalIndex"></param>
	/// <returns> : Index가 valid하지 않다면 return false </returns>
	bool StopFocusedAnimation(uint8 GoalIndex);

	/// <summary>
	/// <para> GoalWidget에서 특정 Goal 내용 부분의 수를 업데이트할 때 사용 </para>
	/// <para> 공교롭게도 WeaponStage의 ADS 모드 사격 표적지 명중 수만 Update하면 되어서 해당 GoalWidget에서만 Event를 구현 </para>
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateNumberInfoOnGoalWidget(uint8 UpdatedNumber);

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (BindWidgetAnim), Transient, Category = "Animations")
	UWidgetAnimation* StageStartAnimation{};

private:
	
	TArray<UWidgetAnimation*> GoalFocusedAnimations{};

	TArray<UWidgetAnimation*> GoalSucceededAnimations{};

private: // Animation 재생 이후로 Widget들의 초기화를 처리해야 함 / 따라서 Widget 객체들 저장

	/*TArray<class UImage*> FocusedBackgroundImages{};

	TArray<class UOverlay*> CheckBoxes{};

	TArray<UImage*> CompletedWhiteImages{};

	TArray<UImage*> CompletedBoxImages{};

	TArray<UImage*> CheckedBoxImages{};

	TArray<UImage*> CheckedBoxEffectImages{};

	TArray<UOverlay*> Overlays{};*/
};
