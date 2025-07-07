// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_TutorialGoalExplanation.generated.h"

/// <summary>
/// Tutorial Goal 단위로 상단 중앙에 띄우는 Goal Explanation panel class
/// </summary>
UCLASS()
class UE_PUBG_API UC_TutorialGoalExplanation : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	
	/// <summary>
	/// 해당 Goal의 Explanation 시작하기 
	/// </summary>
	void StartExplanation();

	/// <summary>
	/// 해당 Goal의 Explanation 멈추기
	/// </summary>
	void StopExplanation();

private:

	// 실질적인 Explanation 내용 Panel들, Sequence 순서대로 집어넣고 사용
	TArray<class UCanvasPanel*> ExplanationPanels{};

	// 검정색 Gradation Background Images
	TArray<class UImage*> BackgroundImages{};

	// Explanation Panel의 마지막 Index
	int LastIndex = -1;

	uint8 CurrentActivatedIndex{};

	// 현재 설명문의 총 Activated 시간 체크
	float ActivatedTimer{};
};
