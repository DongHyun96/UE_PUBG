// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_GameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetOwnerPlayer(class AC_Player* InOwnerPlayer) { OwnerPlayer = InOwnerPlayer; }

	void ActivateWinningSequence();

	void ActivateLoseSequence();

private:

	void HandleExitToLobbyTimer(float DeltaTime);

private:

	AC_Player* OwnerPlayer{};

protected:

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* WinningChickenAnimation{};

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* LoseAnimation{};	

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerCharacterName{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RankingText{}; // 중단의 RankingText ( #(RankingNumber) )

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillCountText{};
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RankingTextTopRight{}; // Default "1"로 세팅되어 있음
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RankingTextTotalCharacterCount{};
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExitToLobbyCountDownText{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<UWorld> LobbyLevel{};
	
private:

	// 최대 60초 기다린 이후 로비로 나가기
	float ExitToLobbyTimer = 60.f;
	
};
