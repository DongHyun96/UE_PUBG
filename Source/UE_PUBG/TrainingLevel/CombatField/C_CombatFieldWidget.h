// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_EnemyCombatFieldManager.h"
#include "Blueprint/UserWidget.h"
#include "C_CombatFieldWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_CombatFieldWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void SetOwnerCombatFieldManager(class AC_CombatFieldManager* InCombatFieldManager) { OwnerCombatFieldManager = InCombatFieldManager; }

	void SetSpectatorInfoVisibility(bool Visible) { SpectatingInfoPanelOpacityDestination = Visible ? 1.f : 0.f; }

	/// <summary>
	/// SpectatorType에 따른 SpectatorInfoText 세팅
	/// </summary>
	/// <param name="InSpectatorType"></param>
	void SetSpectatorInfoText(ESpectatorType InSpectatorType);

	class UCanvasPanel* GetCombatSimulationPanel() const { return CombatSimulationPanel; }

private:
	
	/// <summary>
	/// SpectatorInfo 패널 및 TextBlock Flickering 처리 
	/// </summary>
	void HandleSpectatorInfo(float InDeltaTime);
	
private:

	AC_CombatFieldManager* OwnerCombatFieldManager{};

protected:

	UPROPERTY(meta=(BindWidget))
	class UC_PlayerCombatFieldWidget* PlayerCombatFieldWidget{};

protected:

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* CombatSimulationPanel{};

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* SpectatingInfoPanel{};

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* SpectatorInfoText{};

private:

	const TMap<ESpectatorType, FString> SpectatorInfoTextMap =
	{
		{ESpectatorType::Enemy1, "Spectating CombatTester1"},
		{ESpectatorType::Enemy2, "Spectating CombatTester2"},
		{ESpectatorType::Free,	 "Spectating Free camera"}
	};

	float SpectatingInfoPanelOpacityDestination{};

};
