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
	/// MagazineText �����ϱ�
	/// </summary>
	/// <param name="Ammo"> : ź�� ���� </param>
	/// <param name="bHasEffect"> : ź�� text ������Ʈ ȿ���� �������� (ó�� ���� �� ������ false�� �ϱ� -> ��ݽÿ� true�� ��� ����) </param>
	void SetMagazineText(int Ammo, bool bHasEffect = false);

	/// <summary>
	/// LeftAmmo �����ϱ�
	/// </summary>
	/// <param name="Ammo"> : ź�� ���� </param>
	/// <param name="bHasEffect"> : ź�� text ������Ʈ ȿ���� �������� (ó�� ���� �� ������ false�� �ϱ� -> ������ �� true�� ��� ����) </param>
	void SetLeftAmmoText(int Ammo, bool bHasEffect = false);

	void SetVisibility(ESlateVisibility InVisibility) override;
	void SetVisibility(ESlateVisibility InVisibility, bool IsGun);

private:

	/// <summary>
	/// AmmoTextType ������ boilerplate�ڵ� ���� Ammo Text �����ϱ�
	/// </summary>
	/// <param name="TextType"> : ������ Ammo TextType </param>
	/// <param name="Ammo"> : ź�� ���� </param>
	/// <param name="bHasEffect"> : ź�� text ������Ʈ ȿ�� �������� </param>
	void SetAmmoText(EAmmoTextType TextType, int Ammo, bool bHasEffect);

	/// <summary>
	/// Alpha���� ���� �� ShootingModePanel�� Alpha�� ����
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
	/// MagazineText -> ������ ���� or ���� �������� ���� �ٲٱ� (Alpha�� ���� x)
	/// </summary>
	/// <param name="bIsRed"> : ���������� ĥ���� </param>
	void SetCurrentMagazineFontColor(bool bIsRed);

private:

	// ���� �ѱ��� UI���� üũ
	bool bIsGun{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* SingleCanvasPanel{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* BurstCanvasPanel{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCanvasPanel* FullAutoCanvasPanel{};

private:

	// MAX�� ������ �ٷ� ����ֱ�
	EShootingMode CurrentShootingMode{};
	EShootingMode PrevShootingMode{};

private:

	TMap<EShootingMode, TArray<class UImage*>> ShootingModeImages{};
	TMap<EShootingMode, class UCanvasPanelSlot*> ShootingModePanelSlots{};

private: // Initial �� ��Ÿ const �� �� ����
	TMap<EShootingMode, TArray<float>> ShootingModeInitialAlphas{};
	const FVector2D SHOOTINGMODE_INITIAL_POS = { -58.342354f, -0.027027 };
	const float SHOOTING_MODE_XPOS_OFFSET = 40.f; // Initial Pos X���� Start���� ������ ����

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

	// Megazinetexts�� LeftAmmoTexts�� �ʱ�ȭ
	TMap<EAmmoTextType, TArray<class UTextBlock*>> AmmoTexts{};
	const TMap<EAmmoTextType, TPair<float, float>> AMMO_INITIAL_OFFSET_SIZE_PAIRS =
	{
		{EAmmoTextType::MAGAZINE, {35.f, 10.f}},
		{EAmmoTextType::LEFTAMMO, {22.f, 6.f}}
	};


};
