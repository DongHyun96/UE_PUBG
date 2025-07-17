// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ShootingTargetDamageInfoWidget.h"

#include "Components/TextBlock.h"

void UC_ShootingTargetDamageInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Init WidgetAnimTextBlockPairs
	TPair<UWidgetAnimation*, UTextBlock*> Pair = { NormalSpawnAnimation, NormalDamageText };
	WidgetAnimTextBlockPairs.Add(EShootingTargetDamageInfoType::Normal, Pair);

	Pair = {HeadShotSpawnAnimation, HeadShotDamageText};
	WidgetAnimTextBlockPairs.Add(EShootingTargetDamageInfoType::HeadShot, Pair);

	Pair = {GrenadeDamageSpawnAnimation, GrenadeDamageText};
	WidgetAnimTextBlockPairs.Add(EShootingTargetDamageInfoType::Grenade, Pair);
}

void UC_ShootingTargetDamageInfoWidget::Spawn(EShootingTargetDamageInfoType DamageInfoType, float DamageAmount)
{
	// 기존에 재생 중이던 Animation과 겹칠 수 있기 때문에 모두 Stop
	for (uint8 i = 0; i < static_cast<uint8>(EShootingTargetDamageInfoType::Max); ++i)
	{
		EShootingTargetDamageInfoType Type = static_cast<EShootingTargetDamageInfoType>(i);
		StopAnimation(WidgetAnimTextBlockPairs[Type].Key);
	}

	TPair<UWidgetAnimation*, UTextBlock*> Pair = WidgetAnimTextBlockPairs[DamageInfoType];
	PlayAnimation(Pair.Key);
	Pair.Value->SetText(FText::AsNumber(static_cast<int>(DamageAmount)));
}
