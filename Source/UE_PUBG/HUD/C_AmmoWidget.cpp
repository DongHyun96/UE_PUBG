// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_AmmoWidget.h"

#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

#include "Item/Weapon/Gun/C_Gun.h"

#include "Utility/C_Util.h"

void UC_AmmoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentShootingMode = EShootingMode::MAX;

	static TMap<EShootingMode, FString> ImageNames =
	{
		{EShootingMode::SEMI_AUTO,		"Single"},
		{EShootingMode::BURST,			"Burst"},
		{EShootingMode::FULL_AUTO,		"Full"},
		{EShootingMode::SR_SINGLE_SHOT,	"Single"}
	};

	// Init Shooting Mode Images
	for (int i = 0; i < (int)EShootingMode::MAX; i++)
	{
		EShootingMode Mode = static_cast<EShootingMode>(i);

		TArray<float> Alphas{};
		TArray<class UImage*> Images{};

		for (int j = 1; j < 6; j++)
		{
			FString ImageName = ImageNames[Mode] + FString::FromInt(j);
			UImage* TargetImage = Cast<UImage>(GetWidgetFromName(*ImageName));

			if (!TargetImage)
			{
				UC_Util::Print("From AmmoWidget : Fire Mode Image not inited!", FColor::Red, 10.f);
				continue;
			}

			Alphas.Add(TargetImage->GetColorAndOpacity().A);
			Images.Add(TargetImage);
		}

		ShootingModeImages.Add(Mode, Images);
		ShootingModeInitialAlphas.Add(Mode, Alphas);
	}


	ShootingModePanelSlots.Add(EShootingMode::SEMI_AUTO,	Cast<UCanvasPanelSlot>(SingleCanvasPanel->Slot));
	ShootingModePanelSlots.Add(EShootingMode::SR_SINGLE_SHOT,  Cast<UCanvasPanelSlot>(SingleCanvasPanel->Slot));
	ShootingModePanelSlots.Add(EShootingMode::BURST,		Cast<UCanvasPanelSlot>(BurstCanvasPanel->Slot));
	ShootingModePanelSlots.Add(EShootingMode::FULL_AUTO,	Cast<UCanvasPanelSlot>(FullAutoCanvasPanel->Slot));

	// ShootingModePanel Alpha 모두 0으로 초기화
	for (int i = 0; i < (int)EShootingMode::MAX; i++)
	{
		EShootingMode Mode = static_cast<EShootingMode>(i);
		ShootingModePanelSlots[Mode]->SetPosition(SHOOTINGMODE_INITIAL_POS + FVector2D::UnitX() * 10.f);
		SetShootingModePanelAlpha(Mode, 0.f);
	}

	/*===========================================================================================================*/

	AmmoTexts.Add(EAmmoTextType::MAGAZINE, MagazineTexts);
	AmmoTexts.Add(EAmmoTextType::LEFTAMMO, LeftAmmoTexts);

	// 모든 text 색상의 alpha값 0.f로 초기화
	for (auto& T : MagazineTexts) SetFontAlpha(T, 0.f);
	for (auto& T : LeftAmmoTexts) SetFontAlpha(T, 0.f);
}

void UC_AmmoWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	HandleShootingModeUIUpdate(DeltaTime);
	HandleAmmoUIUpdate(EAmmoTextType::MAGAZINE, DeltaTime);
	HandleAmmoUIUpdate(EAmmoTextType::LEFTAMMO, DeltaTime);
}

void UC_AmmoWidget::SetShootingMode(EShootingMode Mode)
{
	if (Mode == EShootingMode::MAX)
	{
		UC_Util::Print("From UC_AmmoWidget::SetShootingMode - Parameter max mode!", FColor::Red, 10.f);
		return;
	}

	// 현재 CurrentShootingMode와 setting값이 같다면 넘어감
	if (CurrentShootingMode == Mode) return;

	// 모든 이미지 alpha 모두 0으로 초기화 처리
	for (int i = 0; i < (int)EShootingMode::MAX; i++)
		SetShootingModePanelAlpha(static_cast<EShootingMode>(i), 0.f);

	// 처음 Setting되는 경우
	if (CurrentShootingMode == EShootingMode::MAX)
	{
		CurrentShootingMode = Mode;
		
		// 들어온 Mode와 다르게 PrevShootingMode 세팅시키기
		// Single_Shot과 Semi_auto의 이미지가 동일하기 때문에 생기는 버그 방지
		if (CurrentShootingMode == EShootingMode::SR_SINGLE_SHOT)
			PrevShootingMode = EShootingMode::FULL_AUTO;
		else
		{
			for (int i = 0; i < static_cast<int>(EShootingMode::MAX); i++)
			{
				EShootingMode _mode = static_cast<EShootingMode>(i);
				if (Mode != _mode)
				{
					PrevShootingMode = _mode;
					break;
				}
			}
		}

		// 도착 지점으로 바로 보내버리기
		ShootingModePanelSlots[PrevShootingMode]->SetPosition(SHOOTINGMODE_INITIAL_POS - FVector2D::UnitX() * SHOOTING_MODE_XPOS_OFFSET);
		ShootingModePanelSlots[CurrentShootingMode]->SetPosition(SHOOTINGMODE_INITIAL_POS);
		return;
	}

	PrevShootingMode	= CurrentShootingMode;
	CurrentShootingMode = Mode;

	// Prev Mode Position & Alpha initing
	SetShootingModePanelAlpha(PrevShootingMode, 1.f);
	ShootingModePanelSlots[PrevShootingMode]->SetPosition(SHOOTINGMODE_INITIAL_POS);

	// Current Mode Position initing
	FVector2D StartPos = SHOOTINGMODE_INITIAL_POS + FVector2D::UnitX() * SHOOTING_MODE_XPOS_OFFSET;
	ShootingModePanelSlots[CurrentShootingMode]->SetPosition(StartPos);

}

void UC_AmmoWidget::SetMagazineText(int Ammo, bool bHasEffect)
{
	SetCurrentMagazineFontColor(Ammo <= 0); 
	SetAmmoText(EAmmoTextType::MAGAZINE, Ammo, bHasEffect);
}

void UC_AmmoWidget::SetLeftAmmoText(int Ammo, bool bHasEffect)
{
	bLeftAmmoZero = (Ammo <= 0);
	SetAmmoText(EAmmoTextType::LEFTAMMO, Ammo, bHasEffect);
}

void UC_AmmoWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (InVisibility == ESlateVisibility::Hidden)
		CurrentShootingMode = EShootingMode::MAX;
}

void UC_AmmoWidget::SetVisibility(ESlateVisibility InVisibility, bool IsGun)
{
	Super::SetVisibility(InVisibility);
	bIsGun = IsGun;

	if (!IsGun)
	{
		// ShootingModePanel Alpha 모두 0으로 초기화
		for (int i = 0; i < (int)EShootingMode::MAX; i++)
		{
			EShootingMode Mode = static_cast<EShootingMode>(i);
			SetShootingModePanelAlpha(Mode, 0.f);
		}

		// LeftAmmoText Alpha 0으로 세팅
		for (auto& T : LeftAmmoTexts) SetFontAlpha(T, 0.f);
	}

	// Hidden이 이 함수로 들어올 때 버그를 방지하기 위함
	if (InVisibility == ESlateVisibility::Hidden)
		CurrentShootingMode = EShootingMode::MAX;
	
}

void UC_AmmoWidget::SetAmmoText(EAmmoTextType TextType, int Ammo, bool bHasEffect)
{
	UTextBlock* PrevTextBlock		= AmmoTexts[TextType][PREV_INDEX];
	UTextBlock* CurrentTextBlock	= AmmoTexts[TextType][CURRENT_INDEX];
	const float FONT_SIZE_BASE		= AMMO_INITIAL_OFFSET_SIZE_PAIRS[TextType].Key;
	const float FONT_SIZE_OFFSET	= AMMO_INITIAL_OFFSET_SIZE_PAIRS[TextType].Value;

	bool bLeftAmmoAndIsZero = TextType == EAmmoTextType::LEFTAMMO && bLeftAmmoZero;

	// 전환 효과 없이 바로 UI에 띄우기
	if (!bHasEffect || bLeftAmmoAndIsZero) 
	{
		// Prev Text setting
		SetFontAlpha(PrevTextBlock, 0.f);
		SetFontSize(PrevTextBlock, FONT_SIZE_BASE - FONT_SIZE_OFFSET);

		// Current Text setting
		float Alpha = bLeftAmmoAndIsZero ? 0.f : 1.f;
		SetFontAlpha(CurrentTextBlock, Alpha);
		CurrentTextBlock->SetText(FText::AsNumber(Ammo));
		SetFontSize(CurrentTextBlock, FONT_SIZE_BASE);

		return;
	}

	// Prev를 현재의 Current 내용으로 수정, Current는 새로 들어온 내용으로 수정
	PrevTextBlock->SetText(CurrentTextBlock->GetText());
	CurrentTextBlock->SetText(FText::AsNumber(Ammo));

	// Lerp 출발 지점으로 수정 / Alpha는 Font 크기에 따라 Tick의 Lerp로 처리 예정
	SetFontSize(PrevTextBlock, FONT_SIZE_BASE);
	SetFontSize(CurrentTextBlock, FONT_SIZE_BASE + FONT_SIZE_OFFSET);
}

void UC_AmmoWidget::SetShootingModePanelAlpha(EShootingMode Mode, float Alpha)
{
	if (Mode == EShootingMode::MAX) return;

	for (int i = 0; i < ShootingModeImages[Mode].Num(); i++)
	{
		UImage* Image		= ShootingModeImages[Mode][i];
		float InitialAlpha	= ShootingModeInitialAlphas[Mode][i];

		FLinearColor ImageColor = Image->GetColorAndOpacity();
		float NewAlpha			= FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f), FVector2D(0.f, InitialAlpha), Alpha);
		ImageColor.A			= NewAlpha;

		Image->SetColorAndOpacity(ImageColor);
	}
}

void UC_AmmoWidget::HandleShootingModeUIUpdate(const float& DeltaTime)
{
	if (!bIsGun) return;

	static const float LERP_SPEED = 10.f;

	// 빼기가 왼쪽
	// PrevShootingMode와 CurrentShootingMode 위치 및 Alpha값 처리하기
	float PrevModeCurX	= ShootingModePanelSlots[PrevShootingMode]->GetPosition().X;
	float PrevModeDestX = SHOOTINGMODE_INITIAL_POS.X - SHOOTING_MODE_XPOS_OFFSET;
	float Y				= SHOOTINGMODE_INITIAL_POS.Y;

	// Prev - 만일 이미 도착 지점에 도달했을 때
	if (FMath::Abs(PrevModeCurX - PrevModeDestX) < 1.f)
	{
		ShootingModePanelSlots[PrevShootingMode]->SetPosition(FVector2D(PrevModeDestX, Y));
		SetShootingModePanelAlpha(PrevShootingMode, 0.f);
	}
	else
	{
		float ArrivedX = FMath::Lerp(PrevModeCurX, PrevModeDestX, DeltaTime * LERP_SPEED);
		ShootingModePanelSlots[PrevShootingMode]->SetPosition(FVector2D(ArrivedX, Y));
		float ArrivedAlpha = FMath::GetMappedRangeValueClamped
		(
			FVector2D(SHOOTINGMODE_INITIAL_POS.X, PrevModeDestX),
			FVector2D(1.f, 0.f),
			ArrivedX
		);
		SetShootingModePanelAlpha(PrevShootingMode, ArrivedAlpha);
	}

	float CurModeCurX	= ShootingModePanelSlots[CurrentShootingMode]->GetPosition().X;
	float CurModeDestX	= SHOOTINGMODE_INITIAL_POS.X;
	
	if (FMath::Abs(CurModeCurX - CurModeDestX) < 1.f)
	{
		ShootingModePanelSlots[CurrentShootingMode]->SetPosition(FVector2D(CurModeDestX, Y));
		SetShootingModePanelAlpha(CurrentShootingMode, 1.f);
	}
	else
	{
		float ArrivedX = FMath::Lerp(CurModeCurX, CurModeDestX, DeltaTime * LERP_SPEED);
		ShootingModePanelSlots[CurrentShootingMode]->SetPosition(FVector2D(ArrivedX, Y));
		float ArrivedAlpha = FMath::GetMappedRangeValueClamped
		(
			FVector2D(CurModeDestX + SHOOTING_MODE_XPOS_OFFSET, CurModeDestX),
			FVector2D(0.f, 1.f),
			ArrivedX
		);
		SetShootingModePanelAlpha(CurrentShootingMode, ArrivedAlpha);
	}
}

void UC_AmmoWidget::HandleAmmoUIUpdate(const EAmmoTextType& TextType, const float& DeltaTime)
{
	if (TextType == EAmmoTextType::LEFTAMMO && !bIsGun) return;

	UTextBlock* PrevTextBlock		= AmmoTexts[TextType][PREV_INDEX];
	UTextBlock* CurrentTextBlock	= AmmoTexts[TextType][CURRENT_INDEX];
	const float FONT_SIZE_BASE		= AMMO_INITIAL_OFFSET_SIZE_PAIRS[TextType].Key;
	const float FONT_SIZE_OFFSET	= AMMO_INITIAL_OFFSET_SIZE_PAIRS[TextType].Value;
	const float PREV_FONTSIZE_DEST	= FONT_SIZE_BASE - FONT_SIZE_OFFSET;

	static const float LERP_SPEED	= 7.f;
	

	float PrevFontSize = PrevTextBlock->GetFont().Size;

	if (FMath::Abs(PrevFontSize - PREV_FONTSIZE_DEST) < 0.5f)
	{
		SetFontSize(PrevTextBlock, PREV_FONTSIZE_DEST);
		SetFontAlpha(PrevTextBlock, 0.f);
	}
	else // Prev Lerp 처리
	{
		PrevFontSize = FMath::Lerp(PrevFontSize, PREV_FONTSIZE_DEST, DeltaTime * LERP_SPEED);
		SetFontSize(PrevTextBlock, PrevFontSize);
		float Alpha = FMath::GetMappedRangeValueClamped
		(
			FVector2D(FONT_SIZE_BASE, PREV_FONTSIZE_DEST),
			FVector2D(1.f, 0.f),
			PrevFontSize
		);
		SetFontAlpha(PrevTextBlock, Alpha);
	}

	float CurrentFontSize = CurrentTextBlock->GetFont().Size;

	if (FMath::Abs(CurrentFontSize - FONT_SIZE_BASE) < 0.5f)
	{
		SetFontSize(CurrentTextBlock, FONT_SIZE_BASE);

		float Alpha = (TextType == EAmmoTextType::LEFTAMMO && bLeftAmmoZero) ? 0.f : 1.f;
		SetFontAlpha(CurrentTextBlock, Alpha);
	}
	else // Current Lerp 처리
	{
		CurrentFontSize = FMath::Lerp(CurrentFontSize, FONT_SIZE_BASE, DeltaTime * LERP_SPEED);
		SetFontSize(CurrentTextBlock, CurrentFontSize);
		float Alpha = FMath::GetMappedRangeValueClamped
		(
			FVector2D(FONT_SIZE_BASE + FONT_SIZE_OFFSET, FONT_SIZE_BASE),
			FVector2D(0.f, 1.f),
			CurrentFontSize
		);
		SetFontAlpha(CurrentTextBlock, Alpha);
	}
}

void UC_AmmoWidget::SetFontSize(UTextBlock* TextBlock, const float& InSize)
{
	FSlateFontInfo FontInfo = TextBlock->GetFont();
	FontInfo.Size = InSize;
	TextBlock->SetFont(FontInfo);
}

void UC_AmmoWidget::SetFontAlpha(UTextBlock* TextBlock, const float& InAlpha)
{
	FLinearColor Color = TextBlock->GetColorAndOpacity().GetSpecifiedColor();
	Color.A = InAlpha;
	TextBlock->SetColorAndOpacity(FSlateColor(Color));
	
	Color = TextBlock->GetShadowColorAndOpacity();
	Color.A = InAlpha;
	TextBlock->SetShadowColorAndOpacity(Color); // Shadow Alpha값도 지정
}

void UC_AmmoWidget::SetCurrentMagazineFontColor(bool bIsRed)
{
	static float INITIAL_COLOR_GRAYSCALE_VAL = 0.71875f;

	FLinearColor Color = MagazineTexts[CURRENT_INDEX]->GetColorAndOpacity().GetSpecifiedColor();
	
	if (!bIsRed)
	{
		Color.R = INITIAL_COLOR_GRAYSCALE_VAL;
		Color.G = INITIAL_COLOR_GRAYSCALE_VAL;
		Color.B = INITIAL_COLOR_GRAYSCALE_VAL;
	}
	else
	{
		Color.R = 1.f;
		Color.G = 0.f;
		Color.B = 0.f;
	}

	MagazineTexts[CURRENT_INDEX]->SetColorAndOpacity(FSlateColor(Color));
}



