// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_ShootingTargetDamageInfoWidget.generated.h"

enum class EShootingTargetDamageInfoType : uint8
{
	Normal,		// 일반 부위 총기 피격
	HeadShot,	// 총기 HeadShot 피격
	Grenade,		// Grenade 피격
	Max
};

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ShootingTargetDamageInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

public:
	
	/// <summary>
	/// Damage 정보 Widget 스폰시키기 
	/// </summary>
	/// <param name="bIsHeadShot"> : HeadShot 여부 </param>
	/// <param name="DamageAmount"> : Damage 총량 </param>
	void Spawn(EShootingTargetDamageInfoType DamageInfoType, float DamageAmount);

protected:

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NormalSpawnAnimation{};
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HeadShotSpawnAnimation{};

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* GrenadeDamageSpawnAnimation{};
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NormalDamageText{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HeadShotDamageText{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadeDamageText{};

private:

	TMap<EShootingTargetDamageInfoType, TPair<UWidgetAnimation*, UTextBlock*>> WidgetAnimTextBlockPairs{};
	

};
