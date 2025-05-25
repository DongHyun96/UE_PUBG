// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_ToggleButtonWidget.generated.h"

enum class ELevelType : uint8;

enum class EToggleState : uint8
{
	Idle,
	Selected
};

enum class EHorizontalBoxPlace : uint8
{
	FarLeft,
	Middle,
	FarRight
};

/**
 * 특정 케이스 - Lobby UI의 Map Selection에서의 Toggle 그룹 버튼
 * (위의 상황에서만 쓰일 예정)
 */
UCLASS()
class UE_PUBG_API UC_ToggleButtonWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativePreConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetParentToggleButtonGroup(class UC_ToggleButtonGroupWidget* InParentToggleButtonGroup) { ParentToggleButtonGroup = InParentToggleButtonGroup; }

	void SetToggleState(EToggleState InState) { ToggleState = InState; }
	EToggleState GetToggleState() const { return ToggleState; }

	void SetHorizontalBoxPlace(EHorizontalBoxPlace InHorizontalBoxPlace) { HorizontalBoxPlace = InHorizontalBoxPlace; }
	EHorizontalBoxPlace GetHorizontalBoxPlace() const { return HorizontalBoxPlace; }

	const FMargin& GetIdleMargin() const { return HorizontalBoxPlaceMargins[HorizontalBoxPlace]; }

	ELevelType GetLevelType() const { return LevelType; }

public:
	
	/// <summary>
	/// 버튼 골렸을 때 처리
	/// </summary>
	void Select();
	
	/// <summary>
	/// Selected 상태에서 UnSelected 처리되었을 때의 처리 
	/// </summary>
	void OnUnSelected();
	
protected:

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:

	// 이 ToggleButton이 속해있는 그룹
	UC_ToggleButtonGroupWidget* ParentToggleButtonGroup{};
	
private:

	EToggleState ToggleState{};
	EHorizontalBoxPlace HorizontalBoxPlace{};

protected:

	// 이 버튼이 Mapping된 LevelType
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ELevelType LevelType{};

protected:

	UPROPERTY(meta = (BindWidget))
	class UBorder* MapImageBorder{};
	
	UPROPERTY(meta = (BindWidget))
	class UImage* MapImage{};

	UPROPERTY(meta = (BindWidget))
	UImage* MapNameBGBlur{};

	UPROPERTY(meta = (BindWidget))
	UImage* BarImage{};

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MapName{};

protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* MapImageTexture{};
	
private:

	FVector2D MapImageScaleDest = FVector2D(1.f, 1.f);
	

private:

	// false(not hovered), true Hovered Left offset
	static const TMap<bool, FMargin> HoverMapNameMargins;

	// Horizontal Box 내의 위치별 selected 당하지 않았을 때의 Margin들
	static const TMap<EHorizontalBoxPlace, FMargin> HorizontalBoxPlaceMargins; 
};


