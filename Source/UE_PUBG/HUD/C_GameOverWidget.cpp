// Fill out your copyright notice in the Description page of Project Settings.


#include "C_GameOverWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Character/C_Player.h"
#include "Character/Component/C_PlayerController.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Singleton/C_GameSceneManager.h"
#include "Slate/SGameLayerManager.h"
#include "Utility/C_Util.h"

void UC_GameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/*if (!WinningChickenAnimation)			UC_Util::Print("WinningChickenAnimation not initialized!", FColor::Red, 10.f);
	if (!PlayerCharacterName)			UC_Util::Print("PlayerCharacterName not initialized!", FColor::Red, 10.f);
	if (!RankingTextTopRight)				UC_Util::Print("RankingTextTopRight not initialized!", FColor::Red, 10.f);
	if (!RankingTextTotalCharacterCount)	UC_Util::Print("RankingTextTotalCharacterCount not initialized!", FColor::Red, 10.f);*/
}

void UC_GameOverWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	HandleExitToLobbyTimer(InDeltaTime);
}

void UC_GameOverWidget::ActivateWinningSequence()
{
	this->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
	
	PlayerCharacterName->SetText(FText::FromString(Player->GetCharacterName()));
	
	FString TotalCharacterString = " / " + FString::FromInt(GAMESCENE_MANAGER->GetTotalPlayedCharacterCount());
	
	RankingTextTotalCharacterCount->SetText(FText::FromString(TotalCharacterString));
	RankingText->SetText(FText::FromString("#1"));

	KillCountText->SetText(FText::FromString(FString::FromInt(Player->GetKillCount())));

	PlayAnimation(WinningChickenAnimation);
}

void UC_GameOverWidget::ActivateLoseSequence()
{
	this->SetVisibility(ESlateVisibility::Visible);

	AC_PlayerController* PlayerController = Cast<AC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	

	PlayerController->SetInputMode
	(
		FInputModeGameAndUI().
		SetWidgetToFocus(this->TakeWidget()).
		SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock).
		SetHideCursorDuringCapture(false)
	);

	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetIgnoreLookInput(true);

	AC_Player* Player = GAMESCENE_MANAGER->GetPlayer();
	PlayerCharacterName->SetText(FText::FromString(Player->GetCharacterName()));
	FString TotalCharacterString = " / " + FString::FromInt(GAMESCENE_MANAGER->GetTotalPlayedCharacterCount());
	RankingTextTotalCharacterCount->SetText(FText::FromString(TotalCharacterString));

	FString RankingString = "#" + FString::FromInt(Player->GetRanking());
	RankingText->SetText(FText::FromString(RankingString));

	KillCountText->SetText(FText::FromString(FString::FromInt(Player->GetKillCount())));

	PlayAnimation(LoseAnimation);
}

void UC_GameOverWidget::HandleExitToLobbyTimer(float DeltaTime)
{
	if (ExitToLobbyCountDownText->GetRenderOpacity() < 0.95f) return;

	ExitToLobbyTimer -= DeltaTime;

	int Sec = static_cast<int>(ExitToLobbyTimer) + 1;
	FString CountDownString = "You will exit to lobby in " + FString::FromInt(Sec) + " seconds";
	ExitToLobbyCountDownText->SetText(FText::FromString(CountDownString));

	if (ExitToLobbyTimer < 0.f)
	{
		// TODO : 로비로 나가기
		
	}
}
