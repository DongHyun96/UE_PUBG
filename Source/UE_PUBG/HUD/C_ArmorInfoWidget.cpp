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
			UC_Util::Print("From ArmorInfoWidget : BackPack name mismatched!", FColor::Red, 10.f);
			continue;
		}

		BackPackImages.Add(i, TargetImage);
		if (UMaterialInstanceDynamic* Instance = TargetImage->GetDynamicMaterial())
			BackPackMaterialInstanceDynamics.Add(i, Instance);
		else BackPackMaterialInstanceDynamics.Add(i, nullptr); // GetDynamicMaterial() 자체가 없으면 NULL 리터럴 상수값이라 nullptr로 다시금 Add
		
	}

	for (uint8 i = 0; i < 4; ++i)
	{
		FString ImageName = VestNameBase + FString::FromInt(i);
		UImage* TargetImage = Cast<UImage>(GetWidgetFromName(*ImageName));

		if (!TargetImage)
		{
			UC_Util::Print("From ArmorInfoWidget : Vest name mismatched!", FColor::Red, 10.f);
			continue;
		}

		VestImages.Add(i, TargetImage);
		if (UMaterialInstanceDynamic* Instance = TargetImage->GetDynamicMaterial())
			VestMaterialInstanceDynamics.Add(i, Instance);
		else VestMaterialInstanceDynamics.Add(i, nullptr);
	}

	for (uint8 i = 0; i < 4; ++i)
	{
		FString ImageName = HelmetNameBase + FString::FromInt(i);
		UImage* TargetImage = Cast<UImage>(GetWidgetFromName(*ImageName));

		if (!TargetImage)
		{
			UC_Util::Print("From ArmorInfoWidget : Helmet name mismatched!", FColor::Red, 10.f);
			continue;
		}

		HelmetImages.Add(i, TargetImage);
		if (UMaterialInstanceDynamic* Instance = TargetImage->GetDynamicMaterial())
			HelmetMaterialInstanceDynamics.Add(i, Instance);
		else HelmetMaterialInstanceDynamics.Add(i, nullptr);
	}

	SetBackPackInfo(0);
	SetVestInfo(0);
	SetHelmetInfo(0);
}

void UC_ArmorInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

bool UC_ArmorInfoWidget::SetBackPackInfo(uint8 InBackPackLevel, float InCapacityRate)
{
	if (InBackPackLevel > 3 || InCapacityRate < 0.f || InCapacityRate > 1.f)
	{
		UC_Util::Print("From UC_ArmorInfoWidget::SetBackPackInfo : Wrong Range input!", FColor::Red, 10.f);
		return false;
	}

	for (const auto& Pair : BackPackImages)
		Pair.Value->SetVisibility(ESlateVisibility::Hidden);

	BackPackImages[InBackPackLevel]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (InBackPackLevel > 0)
		BackPackMaterialInstanceDynamics[InBackPackLevel]->SetScalarParameterValue("CapacityRate", InCapacityRate);

	CurBackPackLevel = InBackPackLevel;
	
	return true;
}

bool UC_ArmorInfoWidget::SetCurrentBackPackCapacityRate(float InCapacityRate)
{
	if (InCapacityRate < 0.f || InCapacityRate > 1.f)
	{
		UC_Util::Print("From UC_ArmorInfoWidget::SetCurrentBackPackCapacityRate : Wrong Range input!", FColor::Red, 10.f);
		return false;
	}
	if (CurBackPackLevel == 0)
	{
		// UC_Util::Print
		// (
		// 	"From UC_ArmorInfoWidget::SetCurrentBackPackCapacityRate : Cannot Set Capacity on current backPackLevel image!",
		// 	FColor::Red,
		// 	10.f
		// );
		return false;
	}

	BackPackMaterialInstanceDynamics[CurBackPackLevel]->SetScalarParameterValue("CapacityRate", InCapacityRate);
	return true;
}

bool UC_ArmorInfoWidget::SetHelmetInfo(uint8 InHelmetLevel, float InDurabilityRate)
{
	if (InHelmetLevel > 3 || InDurabilityRate < 0.f || InDurabilityRate > 1.f)
	{
		UC_Util::Print("From UC_ArmorInfoWidget::SetHelmetInfo : Wrong Range input!", FColor::Red, 10.f);
		return false;
	}

	for (const auto& Pair : HelmetImages)
		Pair.Value->SetVisibility(ESlateVisibility::Hidden);

	HelmetImages[InHelmetLevel]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (InHelmetLevel > 0)
		HelmetMaterialInstanceDynamics[InHelmetLevel]->SetScalarParameterValue("DurabilityRate", InDurabilityRate);

	CurHelmetLevel = InHelmetLevel;

	return true;
}

bool UC_ArmorInfoWidget::SetCurrentHelmetDurabilityRate(float InDurabilityRate)
{
	if (InDurabilityRate < 0.f || InDurabilityRate > 1.f)
	{
		UC_Util::Print("From UC_ArmorInfoWidget::SetCurrentHelmetDurabilityRate : Wrong Range input!", FColor::Red, 10.f);
		return false;
	}
	if (CurHelmetLevel == 0)
	{
		UC_Util::Print
		(
			"From UC_ArmorInfoWidget::SetCurrentHelmetDurabilityRate : Cannot Set Durability on current HelmetLevel image!",
			FColor::Red,
			10.f
		);
		return false;
	}

	HelmetMaterialInstanceDynamics[CurHelmetLevel]->SetScalarParameterValue("DurabilityRate", InDurabilityRate);
	return true;
}

bool UC_ArmorInfoWidget::SetVestInfo(uint8 InVestLevel, float InDurabilityRate)
{
	if (InVestLevel > 3 || InDurabilityRate < 0.f || InDurabilityRate > 1.f)
	{
		UC_Util::Print("From UC_ArmorInfoWidget::SetVestInfo : Wrong Range input!", FColor::Red, 10.f);
		return false;
	}

	for (const auto& Pair : VestImages)
		Pair.Value->SetVisibility(ESlateVisibility::Hidden);

	VestImages[InVestLevel]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (InVestLevel > 0)
		VestMaterialInstanceDynamics[InVestLevel]->SetScalarParameterValue("DurabilityRate", InDurabilityRate);

	CurVestLevel = InVestLevel;
	
	return true;	
}

bool UC_ArmorInfoWidget::SetCurrentVestDurabilityRate(float InDurabilityRate)
{
	if (InDurabilityRate < 0.f || InDurabilityRate > 1.f)
	{
		UC_Util::Print("From UC_ArmorInfoWidget::SetCurrentVestDurabilityRate : Wrong Range input!", FColor::Red, 10.f);
		return false;
	}
	if (CurVestLevel == 0)
	{
		UC_Util::Print
		(
			"From UC_ArmorInfoWidget::SetCurrentVestDurabilityRate : Cannot Set Durability on current VestLevel image!",
			FColor::Red,
			10.f
		);
		return false;
	}

	VestMaterialInstanceDynamics[CurVestLevel]->SetScalarParameterValue("DurabilityRate", InDurabilityRate);
	return true;
}