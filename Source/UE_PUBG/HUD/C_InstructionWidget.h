// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "C_InstructionWidget.generated.h"

/**
 * 각종 Instruction 및, 킬로그 UI 처리 담당
 */
UCLASS()
class UE_PUBG_API UC_InstructionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

protected:
	
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	void HandleConsumableInstructionFlicker(const float& DeltaTime);
	
	void HandleTextBlockFadeOut(const float& DeltaTime);
	
	void HandlePlayerWarningLogLifeTimes(const float& DeltaTime);
	void HandlePlayerWarningLogPositionsAndDefaultAlpha(const float& DeltaTime);
	
public:

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleEjectInstructionVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleDeployParachuteInstructionVisibility(bool Visible);

	bool ActivateConsumableInstruction(FString UsingTextContent);
	void DeActivateConsumableInstruction() { ConsumableInstruction->SetVisibility(ESlateVisibility::Hidden); }

public:
	/// <summary>
	/// 자기장 페이즈 및 자기장 관련 정보 Text 활성화하기
	/// </summary>
	/// <param name="InstructionString"> : 해당 정보 Text </param>
	/// <returns> : 제대로 활성화 처리되지 않았다면 return false </returns>
	bool ActivateMagneticFieldInstructionText(FString InstructionString);

	/// <summary>
	/// Player Warning Log 추가
	/// </summary>
	/// <param name="WarningLog"> : Warning log </param>
	/// <returns> : 제대로 추가되지 않았다면 return false </returns>
	bool AddPlayerWarningLog(const FString& WarningLog);

private:
	
	/// <summary>
	/// FadeOut 효과 처리 시작하기 
	/// </summary>
	void StartFadeOut(class UTextBlock* TargetTextBlock) { FadeOutTextBlocks.Add(TargetTextBlock); }

protected: // Consumable Item Activating cancel instruction 관련

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UCanvasPanel* ConsumableInstruction{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UTextBlock* ConsumableCurrentUsingTextBlock{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UTextBlock* MagneticFieldStatusInstructionText{};

private:

	// 기본 없어지는 시간 3초
	//Add log -> 넣을 떄에는 Color 체크해서 0인 친구를 바로 사용하면 됨
	// start Alpha 0.75 / 

	TArray<UTextBlock*>			PlayerWarningLogTexts{};
	TArray<UCanvasPanelSlot*>	PlayerWarningLogTextPanels{};
	TArray<FVector2D>			PlayerWarningLogEachPositions{}; // Player Warning Log 각 위치의 초기 Position 값

	// 현재 Log Panel들의 순서 (차례로 밑에서부터 위로)
	TArray<int> PlayerWarningLogSequence{};
	
private:
	TMap<UTextBlock*, float> PlayerWarningLogLifeTimes{}; // Log Spawn된 이 후, FadeOut처리되기 이전까지의 수명

private:
	TSet<UTextBlock*> FadeOutTextBlocks{}; // FadeOut 처리시킬 TextBlock들 (FadeOutStart 처리된 TextBlock 객체들)
};


