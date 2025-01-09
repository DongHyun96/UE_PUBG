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
	/// �������Ʈ �ʿ��� ������ Event Function, HP Bar ������Ʈ�� ���Ѿ� �� �� ȣ��
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateHP(float CurHP);

public: // HUD event ����

	/// <summary>
	/// �������Ʈ �ʿ��� ������ Event Function
	/// </summary>
	/// <param name="HealUpDestHPValue"> : ���� Heal up block�� Dest Value </param>
	/// <param name="TimeRemain"> : �ߵ����� ���� �ð� </param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivatingHealUp(float HealUpDestHPValue, float UsageTime, float UsingTimer);
	//Player->OnActivatingHealUp(HealUpDestHPValue, UsageTime, UsingTimer);

	/// <summary>
	/// �������Ʈ �ʿ��� ������ Event Function
	/// </summary>
	/// <param name="TimeRemain"> : �ߵ����� ���� �ð� </param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivatingBooster(float UsageTime, float UsingTimer);

	/// <summary>
	/// �������Ʈ �ʿ��� ������ Event Function, Activating �ð��� �� á�� �� ȣ�� ������ Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnConsumableItemActivatingEnd();


	/// <summary>
	/// �������Ʈ���� ������ Event Function, Heal Consumable item �� ActivateCompleted State tick���� ȣ���� Event
	/// </summary>
	/// <param name="BlockDestHP"> : ���� Block�� �� ä��� �ѷ� Destination HP </param>
	/// <param name="BlockTimeRemainRate"> : ���� Heal up block�� �ð� ���� ����(0.f ~ 1.f) </param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivateHealUpCompletedTick(float BlockDestHP, float BlockTimeRemainRate);

	/// <summary>
	/// �������Ʈ���� ������ Event Function, consumable item �� Used���� ȣ���� Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnConsumableUsed();


	/// <summary>
	/// �������Ʈ���� ������ Event Function, consumable item �� CancelActivating���� ȣ���� Event
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnCancelActivatingConsumableItem();

	/// <summary>
	/// �������Ʈ���� ������ Event Function, �ν�Ʈ �� ������Ʈ �� ����� Event
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

	class UC_MapWidget* GetMiniMapWidget() const { return MiniMapWidget; }
	class UBorder* GetMiniMapBorder() const { return MiniMapBorder; }
	class UC_MainMapWidget* GetMainMapWidget() const { return MainMapWidget; }
	class UC_OxygenWidget* GetOxygenWidget() const { return OxygenWidget; }
	class UC_SkyDiveWidget* GetSkyDiveWidget() const { return SkyDiveWidget; }
	class UC_InstructionWidget* GetInstructionWidget() const { return InstructionWidget; }

protected:

	static const FVector2D MINIMAP_MINIMIZED_SIZE;
	static const FVector2D MINIMAP_MINIMIZED_POS;

	static const FVector2D MINIMAP_ENLARGED_SIZE;
	static const FVector2D MINIMAP_ENLARGED_POS;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MiniMapLerpSizeDest = MINIMAP_MINIMIZED_SIZE;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MiniMapLerpPosDest = MINIMAP_MINIMIZED_POS;

protected: // Map ����

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_MapWidget* MiniMapWidget{};
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UBorder* MiniMapBorder{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_MainMapWidget* MainMapWidget{};

protected: // Oxygen Widget

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_OxygenWidget* OxygenWidget{};

protected: // SkyDive Widget

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UC_SkyDiveWidget* SkyDiveWidget{};

protected: // Key Instruction Widget

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class  UC_InstructionWidget* InstructionWidget{};


private:

	bool bIsMiniMapEnlarged{};
};
