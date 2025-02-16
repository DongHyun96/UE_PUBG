// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_ArmorInfoWidget.h"

#include "Components/Image.h"
#include "Utility/C_Util.h"

void UC_ArmorInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const FString BackPackNameBase	= "BackPackLv";
	const FString VestNameBase		= "VestLv";
	const FString HelmetNameBase	= "HelmetLv";

	// Armor 이미지들 초기화
	
	for (uint8 i = 0; i < 4; ++i)
	{
		FString ImageName = BackPackNameBase + FString::FromInt(i);
		UImage* TargetImage = Cast<UImage>(GetWidgetFromName(*ImageName));

		if (!TargetImage)
		{
			UC_Util::Print("From ArmoInfoWidget : BackPack name mismatched!", FColor::Red, 10.f);
			continue;
		}

		BackPackImages.Add(i, TargetImage);
	}

	for (uint8 i = 0; i < 4; ++i)
	{
		FString ImageName = VestNameBase + FString::FromInt(i);
		UImage* TargetImage = Cast<UImage>(GetWidgetFromName(*ImageName));

		if (!TargetImage)
		{
			UC_Util::Print("From ArmoInfoWidget : Vest name mismatched!", FColor::Red, 10.f);
			continue;
		}

		VestImages.Add(i, TargetImage);
	}

	for (uint8 i = 0; i < 4; ++i)
	{
		FString ImageName = HelmetNameBase + FString::FromInt(i);
		UImage* TargetImage = Cast<UImage>(GetWidgetFromName(*ImageName));

		if (!TargetImage)
		{
			UC_Util::Print("From ArmoInfoWidget : Helmet name mismatched!", FColor::Red, 10.f);
			continue;
		}

		HelmetImages.Add(i, TargetImage);
	}

	// TODO : Material Instance Dynamic도 초기화 해주기
}

void UC_ArmorInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
