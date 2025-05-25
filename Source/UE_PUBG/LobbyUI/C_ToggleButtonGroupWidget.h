// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_ToggleButtonGroupWidget.generated.h"

/**
 * 특정 케이스 - Lobby UI의 Map Selection에서의 Toggle 버튼들의 그룹으로 사용할 예정
 */
UCLASS()
class UE_PUBG_API UC_ToggleButtonGroupWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	
	/// <summary>
	/// 초기 선택 처리 (NativePreConstruct나 NativeConstruct에서 처리 못하는 부분 때문에 따로 빼 둠) / 부모LobbyWidget의 PreConstruct에서 호출될 예정 
	/// </summary>
	void Init(class UC_LobbyWidget* InParentLobbyWidget);
	
	void SetCurrentSelectedButton(class UC_ToggleButtonWidget* InButton);

	class UC_LobbyWidget* GetParentLobbyWidget() const { return ParentLobbyWidget; }
	
private:
	
	/// <summary>
	/// 재귀 탐색으로 모든 ToggleButtonWidget들 찾아서 초기화
	/// </summary>
	void FindAllToggleButtonWidgets(UWidget* ParentWidget, TArray<UC_ToggleButtonWidget*>& OutArray);

	/// <summary>
	/// 버튼 별 Margin Lerp 조정
	/// </summary>
	/// <param name="DeltaTime"></param>
	void HandleLerpButtonMargins(float DeltaTime);

private:

	FORCEINLINE FMargin LerpMargin(const FMargin& A, const FMargin& B, float Alpha)
	{
		return FMargin
		(
		FMath::Lerp(A.Left,   B.Left,   Alpha),
		FMath::Lerp(A.Top,    B.Top,    Alpha),
		FMath::Lerp(A.Right,  B.Right,  Alpha),
		FMath::Lerp(A.Bottom, B.Bottom, Alpha)
		);
	}

	// void FindParentLobbyWidget();

private:

	UC_ToggleButtonWidget* CurrentSelectedButton{};
	TArray<UC_ToggleButtonWidget*> ToggleButtons{};

	TMap<UC_ToggleButtonWidget*, FMargin> ButtonMarginLerpDests{};

private:

	UC_LobbyWidget* ParentLobbyWidget{};
	
};


