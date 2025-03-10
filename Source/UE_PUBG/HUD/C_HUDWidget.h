// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/CanvasPanel.h"

#include "C_HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_HUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetVisibility(ESlateVisibility InVisibility) override { OtherPanel->SetVisibility(InVisibility); }

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

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnCompassBarPingMarker(FVector WorldPingMarkerLocation);

	UFUNCTION(BlueprintImplementableEvent)
	void HideCompassBarPingMarker();

public:

	void ToggleMiniMapEnlarged();

public:

	class UC_MapWidget*			GetMiniMapWidget()		const { return MiniMapWidget; }
	class UBorder*				GetMiniMapBorder() 		const { return MiniMapBorder; }
	class UC_OxygenWidget*		GetOxygenWidget()		const { return OxygenWidget; }
	class UC_SkyDiveWidget*		GetSkyDiveWidget()		const { return SkyDiveWidget; }
	class UC_InstructionWidget* GetInstructionWidget()	const { return InstructionWidget; }
	class UC_AmmoWidget*		GetAmmoWidget()			const { return AmmoWidget; }
	class  UC_ArmorInfoWidget*	GetArmorInfoWidget()	const { return ArmorInfoWidget; }

protected:

	static const FVector2D MINIMAP_MINIMIZED_SIZE;
	static const FVector2D MINIMAP_MINIMIZED_POS;

	static const FVector2D MINIMAP_ENLARGED_SIZE;
	static const FVector2D MINIMAP_ENLARGED_POS;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MiniMapLerpSizeDest = MINIMAP_MINIMIZED_SIZE;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MiniMapLerpPosDest = MINIMAP_MINIMIZED_POS;

protected: // Map 관련

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_MapWidget* MiniMapWidget{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UBorder* MiniMapBorder{};

protected:

	// Oxygen Widget
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_OxygenWidget* OxygenWidget{};

	// SkyDive Widget
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_SkyDiveWidget* SkyDiveWidget{};

	// Key Instruction Widget
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_InstructionWidget* InstructionWidget{};

	// Armor Info Widget
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_ArmorInfoWidget* ArmorInfoWidget{};

private:

	bool bIsMiniMapEnlarged{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_AmmoWidget* AmmoWidget{};

protected:

	// 항상 상위에 켜져 있을 요소들의 최상위 parent panel (ex - InstructionWidget / HP bar ... )
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* AlwaysOnPanel{};

	// 기타 다른 아이템들 최상위 parent
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* OtherPanel{};



};


