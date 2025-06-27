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
};
