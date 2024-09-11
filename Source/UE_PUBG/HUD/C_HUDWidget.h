// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_HUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/// <summary>
	/// 블루프린트 쪽에서 구현한 Event Function, HP Bar 업데이트를 시켜야 할 때 호출
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateHP(float CurHP);

public: // HUD event 관련

	/// <summary>
	/// 블루프린트 쪽에서 구현한 Event Function
	/// </summary>
	/// <param name="HealUpDestHPValue"> : 현재 Heal up block의 Dest Value </param>
	/// <param name="TimeRemain"> : 발동까지 남은 시간 </param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivatingHealUp(float HealUpDestHPValue, float UsageTime, float UsingTimer);
	//Player->OnActivatingHealUp(HealUpDestHPValue, UsageTime, UsingTimer);

	/// <summary>
	/// 블루프린트 쪽에서 구현한 Event Function
	/// </summary>
	/// <param name="TimeRemain"> : 발동까지 남은 시간 </param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivatingBooster(float UsageTime, float UsingTimer);

	/// <summary>
	/// 블루프린트 쪽에서 구현한 Event Function, Activating 시간이 다 찼을 때 호출 예정인 Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnConsumableItemActivatingEnd();


	/// <summary>
	/// 블루프린트에서 구현한 Event Function, Heal Consumable item 쪽 ActivateCompleted State tick에서 호출할 Event
	/// </summary>
	/// <param name="BlockDestHP"> : 현재 Block의 피 채우기 총량 Destination HP </param>
	/// <param name="BlockTimeRemainRate"> : 현재 Heal up block의 시간 남은 비율(0.f ~ 1.f) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivateHealUpCompletedTick(float BlockDestHP, float BlockTimeRemainRate);

	/// <summary>
	/// 블루프린트에서 구현한 Event Function, consumable item 쪽 Used에서 호출할 Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnConsumableUsed();


	/// <summary>
	/// 블루프린트에서 구현한 Event Function, consumable item 쪽 CancelActivating에서 호출할 Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnCancelActivatingConsumableItem();

	/// <summary>
	/// 블루프린트에서 구현한 Event Function, 부스트 량 업데이트 시 적용될 Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateBoosting(float CurBoosting);

public:

	void ToggleMiniMapEnlarged();

protected:


	const FVector2D MINIMAP_MINIMIZED_SIZE	= { 300.f, 300.f };
	const FVector2D MINIMAP_MINIMIZED_POS	= { -175.921875f, -175.921875f };

	const FVector2D MINIMAP_ENLARGED_SIZE	= { 600.f, 600.f };
	const FVector2D MINIMAP_ENLARGED_POS	= { -329.921875f, -325.081055f };

	UPROPERTY(BlueprintReadOnly)
	FVector2D MiniMapLerpSizeDest = MINIMAP_MINIMIZED_SIZE;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MiniMapLerpPosDest = MINIMAP_MINIMIZED_POS;

private:

	bool bIsMiniMapEnlarged{};
};
