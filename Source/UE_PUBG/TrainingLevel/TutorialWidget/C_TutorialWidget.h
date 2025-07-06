// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrainingLevel/C_TutorialManager.h"
#include "C_TutorialWidget.generated.h"

enum class ETutorialStage : uint8;

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_TutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetTutorialManager(AC_TutorialManager* InTutorialManager) { TutorialManager = InTutorialManager; }
	
	/* Stage Explanation Panel 관련 */
public:

	/// <summary>
	/// TutorialStage에 따라 StageExplanation Panel 내용 세팅하기
	/// </summary>
	/// <param name="TutorialStage"> : TutorialStage </param>
	void SetStageExplanationPanel(ETutorialStage TutorialStage);

	/// <summary>
	/// StageExplanationPanel 감추기/보이기
	/// </summary>
	/// <param name="InIsEnabled"></param>
	void ToggleStageExplanationPanel(bool InIsEnabled);

private:

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void SetSpaceBarProgressBarPercent(float InPercent);

	// Getter를 해당 매크로로 제대로 구현할 수 없어서(레퍼런스 변수형도 불가), c++변수에 SpaceBar Percent를 구해서 대입 처리
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSpaceBarPercent();	

public:
	
	/// <summary>
	/// 현재 Stage의 TutorialGoalWidget 반환 
	/// </summary>
	/// <returns> : 현재 Stage의 TutorialGoalWidget이 존재하지 않다면 return nullptr </returns>
	class UC_TutorialGoalWidget* GetCurrentTutorialGoalWidget() const;
	
	/// <summary>
	/// 해당 TutorialGoalWidget 반환 
	/// </summary>
	/// <param name="TutorialStage"></param>
	/// <returns> : 없다면 return nullptr </returns>
	UC_TutorialGoalWidget* GetTutorialGoalWidget(ETutorialStage TutorialStage) const;

public:
	
	/// <summary>
	/// 현재 Stage의 Tutorial Goal Explanation Widget 반환 
	/// </summary>
	/// <returns> : 현재 Stage의 Tutorial Goal Explanation Widget이 존재하지 않다면 return false </returns>
	class UC_TutorialGoalExplanationContainer* GetCurrentStageGoalExplanationContainer() const;

	/// <summary>
	/// 해당 TutorialStage의 해당 MainGoal에 대한 설명문 Start
	/// </summary>
	/// <param name="TargetTutorialStage"></param>
	/// <param name="TargetGoalIndex"></param>
	/// <returns> : 만약 해당하는 TutorialStage에 대한 설명문 모음(Container)가 없거나, 해당하는 TargetGaolIndex가 valid하지 않으면 return false </returns>
	bool StartTutorialGoalExplanation(ETutorialStage TargetTutorialStage, uint8 TargetGoalIndex);

	/// <summary>
	/// 해당 TutorialStage의 설명문 멈추기
	/// </summary>
	/// <param name="TargetTutorialStage"></param>
	/// <returns> : 해당 TutorialStage의 설명문 모음(Container)가 없다면 return false </returns>
	bool StopTutorialGoalExplanation(ETutorialStage TargetTutorialStage);
	
private:

	AC_TutorialManager* TutorialManager{};
	
	/* Stage Explanation Panel 관련 */

private:

	bool bSpaceBarDown{};
	
protected:

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* StageExplanationPanel{};

	UPROPERTY(BlueprintReadWrite)
	float SpaceBarPercent{};
	
private:

	float StageExplanationPanelOpacityDest{};

protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StageTitle{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UMediaPlayer* MediaPlayer{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<ETutorialStage, class UMediaSource*> MediaSources{};

	UPROPERTY(meta = (BindWidget))
	class UImage* TutorialVideoImage{};
	
private:
	
	TMap<ETutorialStage, UCanvasPanel*> StageExplanations{};	

	const TMap<ETutorialStage, FString> TutorialStageTitles =
	{
		{ETutorialStage::MovementTutorial,	"MOVEMENT TUTORIAL"},
		{ETutorialStage::WeaponTutorial,	"WEAPON TUTORIAL"},
		{ETutorialStage::ThrowableTutorial, "THROWABLE TUTORIAL"},
		{ETutorialStage::HealingTutorial,	"HEALING TUTORIAL"}
	};

	/* MainGoal side checker widget 관련 */

private:
	
	TMap<ETutorialStage, UC_TutorialGoalWidget*> TutorialGoalWidgets{};
	
	const TMap<ETutorialStage, FString> TutorialGoalWidgetNames =
	{
		{ETutorialStage::MovementTutorial,	"MovementTutorialGoalWidget"},
		{ETutorialStage::WeaponTutorial,	"WeaponTutorialGoalWidget"},
		{ETutorialStage::ThrowableTutorial,	"ThrowableTutorialGoalWidget"},
		{ETutorialStage::HealingTutorial,	"HealingTutorialGoalWidget"}
	};

	/* TutorialGoalExplanation Panel(Container) 관련 */
	
private:

	TMap<ETutorialStage, UC_TutorialGoalExplanationContainer*> TutorialGoalExplanations{};

	const TMap<ETutorialStage, FString> TutorialGoalExplanationNames =
	{
		{ETutorialStage::MovementTutorial,	"MovementTutorialGoalExplanation"},
		{ETutorialStage::WeaponTutorial,	"WeaponTutorialGoalExplanation"},
		{ETutorialStage::ThrowableTutorial, "ThrowableTutorialGoalExplanation"},
		{ETutorialStage::HealingTutorial,	"HealingTutorialGoalExplanation"}
	};
	
};
