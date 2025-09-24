// Fill out your copyright notice in the Description page of Project Settings.


#include "C_EnemyHPWidget.h"

#include "Character/C_Enemy.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

void UC_EnemyHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UC_EnemyHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const float NewRenderOpacity = FMath::Lerp(RenderOpacity, WidgetRenderOpacityDest, InDeltaTime * 5.f);
	this->SetRenderOpacity(NewRenderOpacity);
}
