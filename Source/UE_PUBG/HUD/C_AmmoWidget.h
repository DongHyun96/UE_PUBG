// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_AmmoWidget.generated.h"

enum class EShootingMode : uint8;

UCLASS()
class UE_PUBG_API UC_AmmoWidget : public UUserWidget
{
	GENERATED_BODY()

private:

	enum class EAmmoTextType : uint8
	{
		MAGAZINE,
		LEFTAMMO
	};

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:

	void SetShootingMode(EShootingMode Mode);

	/// <summary>
	/// MagazineText 세팅하기
	/// </summary>
	/// <param name="Ammo"> : 탄알 갯수 </param>
	/// <param name="bHasEffect"> : 탄알 text 업데이트 효과를 적용할지 (처음 총을 들 때에는 false로 하기 -> 사격시에 true로 사용 예정) </param>
	void SetMagazineText(int Ammo, bool bHasEffect = false);

	/// <summary>
	/// LeftAmmo 세팅하기
	/// </summary>
	/// <param name="Ammo"> : 탄알 갯수 </param>
	/// <param name="bHasEffect"> : 탄알 text 업데이트 효과를 적용할지 (처음 총을 들 때에는 false로 하기 -> 재장전 시 true로 사용 예정) </param>
	UFUNCTION(BlueprintCallable)
	void SetLeftAmmoText(int Ammo, bool bHasEffect = false);

	void SetVisibility(ESlateVisibility InVisibility) override;
	void SetVisibility(ESlateVisibility InVisibility, bool IsGun);

private:

	/// <summary>
	/// AmmoTextType 통합한 boilerplate코드 정리 Ammo Text 세팅하기
	/// </summary>
	/// <param name="TextType"> : 세팅할 Ammo TextType </param>
	/// <param name="Ammo"> : 탄알 갯수 </param>
	/// <param name="bHasEffect"> : 탄알 text 업데이트 효과 적용할지 </param>
	void SetAmmoText(EAmmoTextType TextType, int Ammo, bool bHasEffect);

	/// <summary>
	/// Alpha값에 따른 각 ShootingModePanel의 Alpha값 지정
	/// </summary>
	/// <param name="Mode"> : Shooting Mode </param>
	/// <param name="Alpha">: 0 ~ 1 </param>
	void SetShootingModePanelAlpha(EShootingMode Mode, float Alpha);

private:

	void HandleShootingModeUIUpdate(const float& DeltaTime);

	void HandleAmmoUIUpdate(const EAmmoTextType& TextType, const float& DeltaTime);

private:

	void SetFontSize(class UTextBlock* TextBlock, const float& InSize);
	void SetFontAlpha(class UTextBlock* TextBlock, const float& InAlpha);

	/// <summary>
	/// MagazineText -> 빨간색 색상 or 기존 색상으로 색상 바꾸기 (Alpha는 수정 x)
	/// </summary>
	/// <param name="bIsRed"> : 빨간색으로 칠할지 </param>
	void SetCurrentMagazineFontColor(bool bIsRed);

private:

	// 현재 총기의 UI인지 체크
	bool bIsGun{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* SingleCanvasPanel{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* BurstCanvasPanel{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* FullAutoCanvasPanel{};

private:

	// MAX일 때에는 바로 띄워주기
	EShootingMode CurrentShootingMode{};
	EShootingMode PrevShootingMode{};

private:

	TMap<EShootingMode, TArray<class UImage*>> ShootingModeImages{};
	TMap<EShootingMode, class UCanvasPanelSlot*> ShootingModePanelSlots{};

private: // Initial 및 기타 const 한 값 관련
	TMap<EShootingMode, TArray<float>> ShootingModeInitialAlphas{};
	const FVector2D SHOOTINGMODE_INITIAL_POS = { -58.342354f, -0.027027 };
	const float SHOOTING_MODE_XPOS_OFFSET = 40.f; // Initial Pos X부터 Start까지 떨어진 간격

/*=======================================================================================*/

private:

	bool bShowAmmoTexts{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<class UTextBlock*> MagazineTexts{};		// idx 0 -> Prev | idx 1 -> Current

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<class UTextBlock*> LeftAmmoTexts{};		// idx 0 -> Prev | idx 1 -> Current

private:

	bool bLeftAmmoZero = true;

	const int PREV_INDEX = 0, CURRENT_INDEX = 1;

	// Megazinetexts와 LeftAmmoTexts로 초기화
	TMap<EAmmoTextType, TArray<class UTextBlock*>> AmmoTexts{};
	const TMap<EAmmoTextType, TPair<float, float>> AMMO_INITIAL_OFFSET_SIZE_PAIRS =
	{
		{EAmmoTextType::MAGAZINE, {35.f, 10.f}},
		{EAmmoTextType::LEFTAMMO, {22.f, 6.f}}
	};


};


