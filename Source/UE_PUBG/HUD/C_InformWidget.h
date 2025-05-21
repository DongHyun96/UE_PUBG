// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Component/C_StatComponent.h"
#include "Components/CanvasPanel.h"
#include "C_InformWidget.generated.h"

enum class EDamageType : uint8;

enum class EQueueLogType : uint8
{
	PlayerWarningLog,
	TopKillFeedLog
};

USTRUCT(BlueprintType)
struct FTopKillFeedBlock // 킬피드 한 block에 대한 struct
{
	GENERATED_BODY()
	
	UCanvasPanel*		Panel{}; // 가장 상단의 부모 CanvasPanel
	UCanvasPanelSlot*	PanelSlot{};

	UCanvasPanel*		KillerNumberPanel{};
	class UTextBlock*	KillerNumberText{};

	UTextBlock*			KillerText{};

	class UImage* 		KillFeedImage{};
	UCanvasPanelSlot*	KillFeedImagePanelSlot{};
	UImage* 			HeadShotImage{};

	UTextBlock* 		VictimText{};
	UTextBlock* 		VictimNumberText{};
};

/**
 * 각종 Instruction 및, 킬로그 UI 처리 담당
 */
UCLASS()
class UE_PUBG_API UC_InformWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	void HandleConsumableInstructionFlicker(const float& DeltaTime);

	/// <summary>
	/// Fade Out Start 처리된 Widget들 FadeOut 처리 
	/// </summary>
	void HandleLogFadeOut(const float& DeltaTime);

	/// <summary>
	/// <para> TextBlockLifeTimers에 들어온 log Text 메시지들의 수명 관리 처리 </para>
	/// <para> 수명이 다 된 Log의 경우, FadeOut 처리까지 담당 후 TextBlockLifeTimers 맵에서 제거 </para>
	/// </summary>
	void HandleLogLifeTimers(const float& DeltaTime);

	/// <summary>
	/// Queue 방식으로 처리되는 로그들에 대한 Effect Handling
	/// </summary>
	/// <param name="DeltaTime"></param>
	/// <param name="QueueLogType"></param>
	void HandleLogQueuePositionsAndDefaultAlpha(const float& DeltaTime, EQueueLogType QueueLogType);
	
	void HandleGameStartTimerPanelDefaultAlpha(const float& DeltaTime);
	void HandleFKeyInstructionPanelDefaultAlpha(const float& InDeltaTime);
	
public:
	
	/// <summary>
	/// FKeyInstruction UI 활성화
	/// </summary>
	/// <param name="Instruction"> : 활성화 시킬 F Instruction Widget 내용 </param>
	void ActivateFKeyInstruction(const FString& Instruction);

	/// <summary>
	/// FKeyInstruction UI 비활성화
	/// </summary>
	void DeActivateFKeyInstruction();

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleEjectInstructionVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleDeployParachuteInstructionVisibility(bool Visible);

	bool ActivateConsumableInstruction(FString UsingTextContent);
	void DeActivateConsumableInstruction() { ConsumableInstruction->SetVisibility(ESlateVisibility::Hidden); }

public:

	void ToggleGameStartTimerVisibility(bool Visible);
	void SetGameStartTimerText(int Second);
	
public:
	/// <summary>
	/// 자기장 페이즈 및 자기장 관련 정보 Text 활성화하기
	/// </summary>
	/// <param name="InstructionString"> : 해당 정보 Text </param>
	/// <returns> : 제대로 활성화 처리되지 않았다면 return false </returns>
	bool ActivateMagneticFieldInstructionText(FString InstructionString);

	/// <summary>
	/// 중간 킬로그 정보 활성화하기
	/// </summary>
	/// <param name="KillFeedDescriptor"></param>
	/// <returns> : 제대로 활성화 처리되지 않았다면 return false </returns>
	bool ActivateMiddleKillFeedLog(const FKillFeedDescriptor& KillFeedDescriptor);

	/// <summary>
	/// Player Warning Log 추가
	/// </summary>
	/// <param name="WarningLog"> : Warning log </param>
	/// <returns> : 제대로 추가되지 않았다면 return false </returns>
	bool AddPlayerWarningLog(const FString& WarningLog);

	/// <summary>
	/// Top Kill Feed 추가
	/// </summary>
	/// <param name="KillFeedDescriptor"></param>
	/// <returns> : 제대로 추가되지 않았다면 return false </returns>
	bool AddTopKillFeedLog(const FKillFeedDescriptor& KillFeedDescriptor);

private:
	
	/// <summary>
	/// Log 메시지가 사라지기까지 걸리는 시간 지정 (이전에 지정된 시간이 있었거나 사라지고 있거나, 사라졌었다면 다시금 수명 재조정 
	/// </summary>
	/// <param name="Log"> : Target Log </param>
	/// <param name="TotalLifeTime"> : FadeOut 처리되기 이전까지의 수명시간 </param>
	void ApplyNewLifeTimerToLog(class UWidget* Log, float TotalLifeTime);
	
	/// <summary>
	/// FadeOut 효과 처리 시작하기 
	/// </summary>
	void StartFadeOut(UWidget* TargetWidget) { FadeOutLogs.Add(TargetWidget); }

protected: // Consumable Item Activating cancel instruction 관련

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UCanvasPanel* ConsumableInstruction{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UTextBlock* ConsumableCurrentUsingTextBlock{};

private:
	
	UTextBlock* MagneticFieldStatusInstructionText{};

private: // F Key instruction baaic 관련

	UCanvasPanel*		FKeyInstructionPanel{};
	UTextBlock*			FKeyInstructionText{};
	UCanvasPanelSlot*	FKeyInstructionPanelSlot{};
	float				FKeyInstructionOpacityLerpDestination{};

private: // Game Start Timer 관련

	UCanvasPanel*	GameStartTimerPanel{};
	UTextBlock*		GameStartTimerText{};
	float			GameStartTimerRenderOpacityLerpDestination{};

private: // Player Warning Log 관련

	TArray<UTextBlock*>			PlayerWarningLogTexts{};
	TArray<UCanvasPanelSlot*>	PlayerWarningLogTextPanels{};
	TArray<FVector2D>			PlayerWarningLogEachPositions{}; // Player Warning Log 각 위치의 초기 Position 값
	TArray<int>					PlayerWarningLogSequence{}; // 현재 Log Panel들의 순서 (차례로 밑에서부터 위로)

private: // Top KillFeed Log 관련

	TArray<FTopKillFeedBlock>	TopKillFeedBlocks{};
	TArray<FVector2D>			TopKillFeedLogEachPositions{};
	TArray<int>					TopKillFeedLogSequence{};

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EDamageType, UTexture2D*> KillLogTextures{};

private: // Middle KillFeed Log 관련

	UCanvasPanel*	MiddleKillFeedPanel{};
	UTextBlock* 	MiddleVictimInfoText{};
	UTextBlock*		MiddleKillCountText{};
	
private:

	TMap<EDamageType, float> DamageTypeKillLogImageLeftRightOffsets =
	{
		{EDamageType::Default,		0.f},
		{EDamageType::Weapon,		0.f},
		{EDamageType::BlueZone,		10.f},
		{EDamageType::Drown,		10.f},
		{EDamageType::Fall,			10.f},
		{EDamageType::VehicleHit,	10.f}
	};
	
private:
	TMap<UWidget*, float>	LogLifeTimers{}; // Log Spawn된 이 후, FadeOut처리되기 이전까지의 수명처리 담당
	TSet<UWidget*>			FadeOutLogs{}; // FadeOut 처리시킬 Widget들

};
