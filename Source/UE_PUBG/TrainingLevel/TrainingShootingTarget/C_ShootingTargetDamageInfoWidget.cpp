// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ShootingTargetDamageInfoWidget.h"

#include "Components/TextBlock.h"

void UC_ShootingTargetDamageInfoWidget::Spawn(bool bIsHeadShot, float DamageAmount)
{
	// 기존에 재생 중이던 Animation과 겹칠 수 있기 때문에 모두 Stop
	StopAnimation(NormalSpawnAnimation);
	StopAnimation(HeadShotSpawnAnimation);

	if (bIsHeadShot)
	{
		HeadShotDamageText->SetText(FText::AsNumber(static_cast<int>(DamageAmount)));
		PlayAnimation(HeadShotSpawnAnimation);
	}
	else
	{
		NormalDamageText->SetText(FText::AsNumber(static_cast<int>(DamageAmount)));
		PlayAnimation(NormalSpawnAnimation);
	}
}
