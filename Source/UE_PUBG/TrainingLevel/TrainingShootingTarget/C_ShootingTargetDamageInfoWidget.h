// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_ShootingTargetDamageInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_ShootingTargetDamageInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	/// <summary>
	/// Damage 정보 Widget 스폰시키기 
	/// </summary>
	/// <param name="bIsHeadShot"> : HeadShot 여부 </param>
	/// <param name="DamageAmount"> : Damage 총량 </param>
	void Spawn(bool bIsHeadShot, float DamageAmount);

protected:

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NormalSpawnAnimation{};
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HeadShotSpawnAnimation{};

protected:
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NormalDamageText{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HeadShotDamageText{};

};
