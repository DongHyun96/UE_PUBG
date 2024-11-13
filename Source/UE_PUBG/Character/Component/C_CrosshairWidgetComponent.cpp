// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Component/C_CrosshairWidgetComponent.h"
#include "UMG.h"
#include "Utility/C_Util.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"

// Sets default values for this component's properties


// Called when the game starts

void UC_CrosshairWidgetComponent::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeAimSightWidget();
	UC_Util::Print("Created Widget");
}

void UC_CrosshairWidgetComponent::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	CountFullAutoFiringTime(InDeltaTime);
	ManageAimWidgetImages();
}


// Called every frame


void UC_CrosshairWidgetComponent::InitializeAimSightWidget()
{

	if (!AimSightWidget)
	{
		UC_Util::Print("No Widget Found");
		return;
	}
	AimSightWidget->AddToViewport();

	AimSightWidget->SetVisibility(ESlateVisibility::Hidden);
	//Crosshair 이미지 변수 저장
	BaseCrosshairImage = Cast<UImage>(AimSightWidget->WidgetTree->FindWidget(FName("Crosshair_Base")));
	if (!BaseCrosshairImage)
	{
		
		UC_Util::Print("NO BASE IMAGE");
	}	
	CrosshairTop    = Cast<UBorder>(AimSightWidget->WidgetTree->FindWidget(FName("CrosshairTop")));
	CrosshairBottom = Cast<UBorder>(AimSightWidget->WidgetTree->FindWidget(FName("CrosshairBottom")));
	CrosshairLeft   = Cast<UBorder>(AimSightWidget->WidgetTree->FindWidget(FName("CrosshairLeft")));
	CrosshairRight  = Cast<UBorder>(AimSightWidget->WidgetTree->FindWidget(FName("CrosshairRight")));
	if (!CrosshairTop)
	{
		UC_Util::Print("NO BASE IMAGE");
	}
	InitialBaseCrosshairImageSlotSize = FVector2D(100, 100);

	RedDotImage = Cast<UImage>(AimSightWidget->WidgetTree->FindWidget(FName("Red_Dot")));
	if (!RedDotImage)
	{
		UC_Util::Print("NO RED DOT");
	}			

	GrayDotImage = Cast<UImage>(AimSightWidget->WidgetTree->FindWidget(FName("Gray_Dot")));
	if (!GrayDotImage)
	{	
		UC_Util::Print("NO GRAY DOT");
	}
	RedDotImage->SetVisibility(ESlateVisibility::Hidden);
	GrayDotImage->SetVisibility(ESlateVisibility::Hidden);
	BaseCrosshairImage->SetVisibility(ESlateVisibility::Hidden);

}

void UC_CrosshairWidgetComponent::SetVisibileOfCrosshairBorder(bool InIsVisible)
{
	if (InIsVisible)
	{
		CrosshairTop->SetVisibility(   ESlateVisibility::SelfHitTestInvisible);
		CrosshairBottom->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CrosshairLeft->SetVisibility(  ESlateVisibility::SelfHitTestInvisible);
		CrosshairRight->SetVisibility( ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		CrosshairTop->SetVisibility(   ESlateVisibility::Hidden);
		CrosshairBottom->SetVisibility(ESlateVisibility::Hidden);
		CrosshairLeft->SetVisibility(  ESlateVisibility::Hidden);
		CrosshairRight->SetVisibility( ESlateVisibility::Hidden);
	}

}

void UC_CrosshairWidgetComponent::CountFullAutoFiringTime(float InDeltaTime)
{
	if (OwnerCharacter->GetIsFiringBullet())
	{
		FullAutoFiringTime += InDeltaTime;
	}
	else
		FullAutoFiringTime = 0;
	FullAutoSpreadRate = FMath::Clamp(1 + FullAutoFiringTime * 5, 1, 3);
}

void UC_CrosshairWidgetComponent::SetCrosshairState(ECrosshairState InState)
{
	if (!AimSightWidget)
	{
		return;
	}
	RedDotImage->SetVisibility(       ESlateVisibility::Hidden);
	BaseCrosshairImage->SetVisibility(ESlateVisibility::Hidden);
	GrayDotImage->SetVisibility(      ESlateVisibility::Hidden);
	SetVisibileOfCrosshairBorder(false);
	switch (InState)
	{
	case ECrosshairState::NORIFLE:
		//BaseCrosshairImage->SetVisibility(ESlateVisibility::Visible);
		GrayDotImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ECrosshairState::RIFLE:
		//BaseCrosshairImage->SetVisibility(ESlateVisibility::Visible);
		SetVisibileOfCrosshairBorder(true);
		GrayDotImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ECrosshairState::RIFLEAIMDOWNSIGHT:
		RedDotImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ECrosshairState::RIFLECRAWLMOVING:
		break;
	default:
		break;
	}
}

void UC_CrosshairWidgetComponent::ToggleWidgetVisibility(bool InOnOff)
{
	bool bIsCharacterMoving = (OwnerCharacter->GetNextSpeed() > 10);
	bool bIsCharacterCrawling = (OwnerCharacter->GetPoseState() == EPoseState::CRAWL);
	if (InOnOff)
	{
		if (bIsCharacterCrawling && bIsCharacterMoving)
			AimSightWidget->SetVisibility(ESlateVisibility::Hidden);
		else
			AimSightWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
		AimSightWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UC_CrosshairWidgetComponent::ManageAimWidgetImages()
{
	AC_Gun* CurGun = Cast<AC_Gun>(OwnerCharacter->GetEquippedComponent()->GetCurWeapon());
	bool OnScreen = (OwnerCharacter->GetNextSpeed() < 600) && OwnerCharacter->GetCanMove();

	if (!IsValid(CurGun))
		SetCrosshairState(ECrosshairState::NORIFLE);
	ToggleWidgetVisibility(OnScreen);
	UpdateImageSize();
}

void UC_CrosshairWidgetComponent::UpdateImageSize()
{

	float SizeMultOnPose = 1.0f;
	float SizeMultOnAiming = 1.0f;
	float BaseStandardValue = 270;
	if (OwnerCharacter->GetIsAimDown())
		SizeMultOnAiming = 0.7;
	EPoseState CurPose = OwnerCharacter->GetPoseState();
	switch (CurPose)
	{
	case EPoseState::STAND:
		break;
	case EPoseState::CROUCH:
		SizeMultOnPose = 0.7;
		BaseStandardValue = 170;
		break;
	case EPoseState::CRAWL:
		SizeMultOnPose = 0.5;
		break;
	case EPoseState::POSE_MAX:
		break;
	default:
		break;
	}

	float CurPlayerSpeed = OwnerCharacter->GetMovementComponent()->Velocity.Size() / 270;
	float SizeMultiplier = FMath::Max(1, CurPlayerSpeed);
	AC_Gun* CurGun = Cast<AC_Gun>(OwnerCharacter->GetEquippedComponent()->GetCurWeapon());
	float BaseSize = 1.0f;
	if (IsValid(CurGun))
	{
		BaseSize = CurGun->GetBaseBulletSpreadDegree();
	}
	UCanvasPanelSlot* ImageSlot = Cast<UCanvasPanelSlot>(BaseCrosshairImage->Slot);
	if (!ImageSlot) return;
	//UC_Util::Print(InitialBaseCrosshairImageSlotSize);
	//UC_Util::Print(BaseSize);

	FVector2D NewSize = FVector2D(100,100) * SizeMultiplier * BaseSize * SizeMultOnPose * SizeMultOnAiming * FullAutoSpreadRate;
	//UC_Util::Print(NewSize, FColor::Blue);

	ImageSlot->SetSize(NewSize);		
	UpdateCrosshairBorderLocation(ImageSlot);
}

void UC_CrosshairWidgetComponent::UpdateCrosshairBorderLocation(UCanvasPanelSlot* InImageSlot)
{
	UCanvasPanelSlot* ImageSlot = InImageSlot;

	FVector2D ImageSize = ImageSlot->GetSize();
	float SpacingLength = ImageSize.X * 0.5f * 0.373f;
	FVector2D Horizontal = FVector2D(SpacingLength, 0);
	FVector2D Vertical   = FVector2D(0, SpacingLength);
	//Top -Y, Bottom +Y, Left -X, Right +X
	Cast<UCanvasPanelSlot>(CrosshairTop->Slot   )->SetPosition(  -Vertical);
	Cast<UCanvasPanelSlot>(CrosshairBottom->Slot)->SetPosition(   Vertical);
	Cast<UCanvasPanelSlot>(CrosshairLeft->Slot  )->SetPosition(-Horizontal);
	Cast<UCanvasPanelSlot>(CrosshairRight->Slot )->SetPosition( Horizontal);

}
