// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "C_EnemyHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_EnemyHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

protected:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	
	void SetOwnerEnemy(class AC_Enemy* InOwnerEnemy) { OwnerEnemy = InOwnerEnemy; }

	void SetHPBarPercent(float InPercent) { HPBar->SetPercent(InPercent); }
	void SetBoostBarPercent(float InPercent) { BoostBar->SetPercent(InPercent); }

	void SetWidgetRenderOpacityDest(float InRenderOpacity) { WidgetRenderOpacityDest = InRenderOpacity; }

private:

	AC_Enemy* OwnerEnemy{};

protected:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar{};

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BoostBar{};

private:

	float WidgetRenderOpacityDest{};
	
};
