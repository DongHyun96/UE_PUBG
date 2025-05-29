// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_LobbyWidget.generated.h"

enum class ELevelType : uint8;

/// <summary>
/// Lobby UI의 위치 파악용
/// </summary>
enum class ELobbyPageLocation : uint8
{
	LogIn,
	MainLobby,
	MapSelectionPanel
};

/**
 * 
 */
UCLASS()
class UE_PUBG_API UC_LobbyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	

public:
	
	void SetCurrentLobbyPageLocation(ELobbyPageLocation InLobbyPageLocation) { CurrentLobbyPageLocation = InLobbyPageLocation; }
	ELobbyPageLocation GetCurrentLobbyPageLocation() const { return CurrentLobbyPageLocation; }

public:
	
	/// <summary>
	/// <para> Map selection ToggleButtonGroup에서 Map Selected 되었을 때, Callback 받는 함수 </para> 
	/// <para> Map Selection panel의 배경 이미지 및 Animation 재생 & 맵 이름 변경 처리 </para> 
	/// </summary>
	/// <param name="InSelected"></param>
	void OnSelectedMapChanged(ELevelType InSelected);

public: // Input 관련 처리
	
	void OnLobbySpaceBarDown();
	void OnLobbyFKeyPressed();
	void OnLobbyFKeyReleased();

protected: // Play Button 처리 관련
	
	/// <summary>
	/// PlayButton이 제대로 눌렸을 때 호출될 Blueprint 구현화 함수 
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlay();

	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<UWorld> GetRandomLevel();

protected:
	
	/// <summary>
	/// Lobby Widget 첫 시작 시, Log in page이면 Main Lobby UI 감추기 용도 
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent)
	void HideMainLobbyUIOnWidgetStart();

private:
	
	/// <summary>
	/// Main Lobby로 진입했을 때 처리해야 할 일련의 과정 (ex. 현재 고른 맵으로 UI 이미지 초기화 등) 
	/// </summary>
	void InitMainLobby();

private: // Log-In page 관련

	UFUNCTION()
	void OnNickNameTextBoxTextChanged(const FText& Text);

	UFUNCTION()
	void OnNickNameTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnLogInConfirmButtonReleased();

private: // Main Lobby 쪽 관련

	/// <summary>
	/// Play button call back
	/// </summary>
	UFUNCTION()
	void OnPlayButtonReleased();

	/// <summary>
	/// Map Selection Left button call back
	/// </summary>
	UFUNCTION()
	void OnLeftButtonReleased();

	/// <summary>
	/// Map Selection Right button call back
	/// </summary>
	UFUNCTION()
	void OnRightButtonReleased();

	/// <summary>
	/// Map Name 하단의 Selected Bars 갱신시키기 
	/// </summary>
	void UpdateSelectedMapBars(ELevelType InSelectedMapType);

private: // SelectGameMapButton 관련
	
	UFUNCTION()
	void OnSelectGameMapButtonReleased();

	UFUNCTION()
	void OnSelectGameMapButtonHovered();

	UFUNCTION()
	void OnSelectGameMapButtonUnhovered();

private: // SettingButton 관련
	
	UFUNCTION()
	void OnSettingButtonReleased();
	UFUNCTION()
	void OnSettingButtonHovered();
	UFUNCTION()
	void OnSettingButtonUnhovered();

private: // Map Selection Panel 버튼 관련

	UFUNCTION()
	void OnMapSelectionPanelConfirmButtonReleased();
	UFUNCTION()
	void OnMapSelectionPanelCancelButtonReleased();

private:

	// 현재 Lobby Page 위치 
	ELobbyPageLocation CurrentLobbyPageLocation{};

	// 현재 Main Lobby UI 토글버튼(스페이스 바)으로 Hidden처리 되었는지
	bool bMainLobbyUIHidden{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<ELevelType, TSoftObjectPtr<UWorld>> Levels{};

protected: // LogIn Page 관련

	UPROPERTY(meta = (BindWidget))
	class UBackgroundBlur* BackgroundBlur{};
	float BlurLerpDest{};

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* NickNameTextBox{};

	UPROPERTY(meta = (BindWidget))
	class UButton* LoginConfirmButton{};

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarningText{};
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ToggleLobbyUIText{};

protected: // MainLobby Play 버튼

	UPROPERTY(meta = (BindWidget))
	UButton* PlayButton{};
	
protected: // MainLobby - Select GameMap 버튼 쪽 관련

	UPROPERTY(meta = (BindWidget))
	class UImage* SelectGameMapImage{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<ELevelType, UTexture2D*> SelectGameMapImageTextures{};
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SelectGameMapButton{};

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* SelectGameMapHoveredPanel{};

protected: // Main Lobby - MapSelectionHorizontalBox 쪽 관련 

	UPROPERTY(meta = (BindWidget))
	UButton* LeftButton{};

	UPROPERTY(meta = (BindWidget))
	UButton* RightButton{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SelectedMapName{};

private: // Main Lobby - MapSelectionHorizontalBox 쪽 관련
	
	// Main Lobby Map Selection Horizontal box의 Map Name 아래의 Bar 이미지들
	TArray<UImage*> MapSelectionBelowBars{};

protected: // Setting button 관련

	UPROPERTY(meta = (BindWidget))
	UButton* SettingButton{};
	
	UPROPERTY(meta = (BindWidget))
	UImage* SettingButtonImageHoverBG{};
	
protected: // Map Selection Panel 관련

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MapSelectionPanel{};
	
	UPROPERTY(meta = (BindWidget))
	class UImage* MapSelectionMapImage{};
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<ELevelType, UTexture2D*> LevelSelectionBackGroundImages{};

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MapSelectedNameText{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MapSelectedSizeText{};

	UPROPERTY(meta = (BindWidget))
	class UC_ToggleButtonGroupWidget* MapSelectToggleButtonGroup{};

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton{};
	
	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton{};

protected: // Widget Animations

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HideLobbyUI{};
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ShowLobbyUI{};
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* BackgroundMapImageAnimation{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* MapNameSizeAnimation{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* LoginPanelSlideIn{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* LoginPanelSlideOut{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* NickNameCharacterWarningAnimation{};
	 
};


