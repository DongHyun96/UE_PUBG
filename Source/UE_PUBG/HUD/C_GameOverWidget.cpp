// Fill out your copyright notice in the Description page of Project Settings.


#include "C_GameOverWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Utility/C_Util.h"

void UC_GameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Init WinnerWinnerChickenDinnerTexts
	/*for (int i = 0; i < 4; ++i)
	{
		FString Str = FString::Printf(TEXT("WWCD_%d"), i);
		FName WidgetName(*Str);
		UTextBlock* TextBlock = Cast<UTextBlock>(GetWidgetFromName(WidgetName));
		if (!TextBlock)
		{
			UC_Util::Print("From GameOverWidget NativeConstruct : WWCD_TextBlock Casting failed!", FColor::Red, 10.f);
			continue;
		}

		WinnerWinnerChickenDinnerTexts.Add(TextBlock);
	}*/
	if (!WinningChickenAnimation)
		UC_Util::Print("WinningChickenAnimation not initialized!", FColor::Red, 10.f);
	else PlayAnimation(WinningChickenAnimation);
}

void UC_GameOverWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
