// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_ArmorInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ArmorInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	
	/// <summary>
	/// BackPack정보 setting하기 
	/// </summary>
	/// <param name="InBackPackLevel"> : 현재 BackPack Level에 대한 정보 (0 ~ 3), 없다면 0 </param>
	/// <param name="InCapacityRate"> : 현재 장착한 Backpack의 용량비 (0 ~ 1) </param>
	/// <returns> : 범위에 맞지 않은 정보가 들어왔다면 return false </returns>
	bool SetBackPackInfo(uint8 InBackPackLevel, float InCapacityRate = 1.f);

	/// <summary>
	/// 현재 Backpack의 CapacityRate 정보 업데이트
	/// </summary>
	/// <param name="InCapacityRate"> : 용량비 (0 ~ 1) </param>
	/// <returns> : 세팅할 수 없는 정보라면 return false </returns>
	bool SetCurrentBackPackCapacityRate(float InCapacityRate);

	/// <summary>
	/// Helmet정보 새로 Setting하기
	/// </summary>
	/// <param name="InHelmetLevel"> : 현재 장착한 Helmet Level (0 ~ 3), 없다면 0 </param>
	/// <param name="InDurabilityRate"> : Helmet 내구도비 (0 ~ 1)</param>
	/// <returns> 범위에 맞지 않은 정보가 들어왔다면 return false </returns>
	bool SetHelmetInfo(uint8 InHelmetLevel, float InDurabilityRate = 1.f);

	/// <summary>
	/// 현재 Helmet의 Durability Rate 정보 업데이트
	/// </summary>
	/// <param name="InDurabilityRate"> : Helmet 내구도비 (0 ~ 1)</param>
	/// <returns> : 세팅할 수 없는 정보라면 return false </returns>
	bool SetCurrentHelmetDurabilityRate(float InDurabilityRate);

	/// <summary>
	/// Vest정보 새로 setting하기
	/// </summary>
	/// <param name="InVestLevel"> : 장착한 Vest의 Level (0 ~ 3), 없다면 0 </param>
	/// <param name="InDurabilityRate"> : 조끼 내구도비 (0 ~ 1) </param>
	/// <returns> 범위에 맞지 않은 정보가 들어왔다면 return false </returns>
	bool SetVestInfo(uint8 InVestLevel, float InDurabilityRate = 1.f);

	/// <summary>
	/// 현재 Vest의 Durability Rate 정보 업데이트
	/// </summary>
	/// <param name="InDurabilityRate"> : Vest 내구도비 (0 ~ 1) </param>
	/// <returns> : 세팅할 수 없는 정보라면 return false </returns>
	bool SetCurrentVestDurabilityRate(float InDurabilityRate);


private:

	uint8 CurBackPackLevel{};
	uint8 CurHelmetLevel{};
	uint8 CurVestLevel{};

private:

	// Level량 & Image Pairs
	TMap<uint8, class UImage*> BackPackImages{};
	TMap<uint8, class UImage*> HelmetImages{};
	TMap<uint8, class UImage*> VestImages{};

private:

	TMap<uint8, class UMaterialInstanceDynamic*> BackPackMaterialInstanceDynamics{};
	TMap<uint8, class UMaterialInstanceDynamic*> HelmetMaterialInstanceDynamics{};
	TMap<uint8, class UMaterialInstanceDynamic*> VestMaterialInstanceDynamics{};
};
