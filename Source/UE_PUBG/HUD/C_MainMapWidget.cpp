// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_MainMapWidget.h"
#include "HUD/C_HUDWidget.h"

#include "Singleton/C_GameSceneManager.h"
#include "Airplane/C_AirplaneManager.h"
#include "Character/C_Player.h"
#include "Character/Component/C_PingSystemComponent.h"
#include "Character/Component/C_SkyDivingComponent.h"

#include "HUD/C_SkyDiveWidget.h"
#include "HUD/C_MapWidget.h"

#include "Utility/C_Util.h"

//#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

const float		UC_MainMapWidget::MAP_SCALE_MAX				= 5.f;
const float		UC_MainMapWidget::SCROLL_DELTA_STEP			= 0.5f;
const float		UC_MainMapWidget::CANVAS_SIZE				= 1080.f;
const FVector2D UC_MainMapWidget::MID_POINT					= { 960.f, 540.f };
const float		UC_MainMapWidget::WORLD_MAP_SIZE			= 100000.f;

const float		UC_MainMapWidget::FIXED_LANDSCAPE_HEIGHT	= 3400.f;
const float		UC_MainMapWidget::PING_BORDER_SIZE			= 30.f;

void UC_MainMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//SetVisibility(ESlateVisibility::Visible);
	//bIsFocusable = true;
	bIsEnabled = true;
}

void UC_MainMapWidget::SetVisibility(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::HitTestInvisible || InVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		UC_Util::Print
		(
			"From UC_MainMapWidget::SetVisibility : HitTestInvisible param in -> Reset it into ESlateVisible::Visible",
			FColor::Red,
			10.f
		);

		InVisibility = ESlateVisibility::Visible;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (InVisibility == ESlateVisibility::Visible)
	{
		UUserWidget::SetVisibility(ESlateVisibility::Visible);

		// ���콺 �Է��� UI��, �ٸ� �Է��� ���ӿ��� ó��
		PC->SetInputMode
		(
			FInputModeGameAndUI()
			.SetWidgetToFocus(this->TakeWidget())
			.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
			.SetHideCursorDuringCapture(false)
		);

		PC->bShowMouseCursor = true;
		PC->SetIgnoreLookInput(true);  // ���콺 �̵��� ���� ī�޶� ȸ���� ����
		//PC->SetIgnoreMoveInput(true);  // ���콺 Ŭ���� ���� �������� ����
		//PC->SetMouseCursorWidget()

		//this->SetUserFocus(PC);

		// Init AirplaneRoute Start Dest pos
		if (AirplaneRouteStartPos.X == 0.f)
		{
			if (AC_AirplaneManager* AirplaneManager = GAMESCENE_MANAGER->GetAirplaneManager())
			{
				TPair<FVector, FVector> StartDestPair = AirplaneManager->GetPlaneRouteStartDestPair();
				SetAirplaneRoute(StartDestPair);
				GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapWidget()->SetAirplaneRoute(StartDestPair);
			}
		}

		// TODO : �� �κ� �ܺη� �ű��
		OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UUserWidget::SetVisibility(ESlateVisibility::Hidden);

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());

		PC->SetIgnoreLookInput(false);  // ���콺 �̵��� ���� ī�޶� ȸ���� ����
		//PC->SetIgnoreMoveInput(false);  // ���콺 Ŭ���� ���� �������� ����

		// TODO : �� �κ� �ܺη� �ű��
		ESkyDivingState PlayerSkyDivingState = OwnerPlayer->GetSkyDivingComponent()->GetSkyDivingState();
		if (PlayerSkyDivingState == ESkyDivingState::SKYDIVING || PlayerSkyDivingState == ESkyDivingState::PARACHUTING)
			OwnerPlayer->GetHUDWidget()->GetSkyDiveWidget()->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UC_MainMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	HandleUpdateMainMapImage(InDeltaTime);
	HandleUpdateMarkers();
}

void UC_MainMapWidget::HandleUpdateMainMapImage(float InDeltaTime)
{
	MainMapScale = FMath::Lerp(MainMapScale, MainMapScaleLerpDest, InDeltaTime * 7.f);
	FVector2D Scale = FVector2D(MainMapScale, MainMapScale);
	MainMapImg->SetRenderScale(Scale);
	GridImg->SetRenderScale(Scale);

	FVector2D MainMapPos = MainMapImg->GetRenderTransform().Translation;
	MainMapPos = FMath::Lerp(MainMapPos, MainMapPosLerpDest, InDeltaTime * 12.f);

	// Border check
	float PosLimit = (MainMapScale * CANVAS_SIZE - CANVAS_SIZE) * 0.5f;
	MainMapPos.X = FMath::Clamp(MainMapPos.X, -PosLimit, PosLimit);
	MainMapPos.Y = FMath::Clamp(MainMapPos.Y, -PosLimit, PosLimit);

	MainMapImg->SetRenderTranslation(MainMapPos);
	GridImg->SetRenderTranslation(MainMapPos);
}

void UC_MainMapWidget::HandleUpdateMarkers()
{
	if (!PlayerMarkerImg)
	{
		UC_Util::Print("From UC_MainMapWidget::HandleUpdateMarkers : PlayerMarker image nullptr.");
		return;
	}

	if (!OwnerPlayer)
	{
		UC_Util::Print("From UC_MainMapWidget::HandleUpdateMarkers : Player member var nullptr.");
		return;
	}

	// ��ġ ���
	FVector2D PlayerPos = { OwnerPlayer->GetActorLocation().Y, -OwnerPlayer->GetActorLocation().X };
	FVector2D PlayerMarkerPos = PlayerPos * (CANVAS_SIZE / WORLD_MAP_SIZE) * MainMapScale;

	//PlayerMarkerPos += MainMapImg->GetRenderTransform().Translation * (1 / MainMapScale);
	PlayerMarkerPos += MainMapImg->GetRenderTransform().Translation;


	PlayerMarkerImg->SetRenderTranslation(PlayerMarkerPos);

	if (!PingMarkerBorder)
	{
		UC_Util::Print("From UC_MainMapWidget::HandleUpdateMarkers : PingMarkerBorder nullptr");
		return;
	}

	FVector2D PingMarkerToMainMapPos = PingMarkerPos * MainMapScale;
	PingMarkerToMainMapPos += MainMapImg->GetRenderTransform().Translation;

	PingMarkerBorder->SetRenderTranslation(PingMarkerToMainMapPos);
}

void UC_MainMapWidget::HandleUpdatePlaneRouteTransform()
{
	FVector2D ImgScale = FVector2D(1.f, 1.f) * MainMapScale;

	// Start Circle �̹��� ��ġ �� ũ�� ���
	FVector2D StartPos = AirplaneRouteStartPos * MainMapScale;
	StartPos += MainMapImg->GetRenderTransform().Translation;
	AirplaneStartCircleImage->SetRenderTranslation(StartPos);
	AirplaneStartCircleImage->SetRenderScale(ImgScale);

	// Dest Triangle �̹��� ��ġ ũ�� ���
	FVector2D DestPos = AirplaneRouteDestPos * MainMapScale;
	DestPos += MainMapImg->GetRenderTransform().Translation;
	AirplaneDestTriangleImage->SetRenderTranslation(DestPos);
	AirplaneDestTriangleImage->SetRenderScale(ImgScale);

	// PlaneRoute �̹��� ��ġ ũ�� ���
	FVector2D PlaneRoutePos = (StartPos + DestPos) * 0.5f;
	AirplaneRouteImage->SetRenderTranslation(PlaneRoutePos);
	AirplaneRouteImage->SetRenderScale(FVector2D(1.f, MainMapScale));

	// PlaneRoute �̹��� ũ�� ����
	if (!AirplaneRouteImageCanvasSlot) AirplaneRouteImageCanvasSlot = Cast<UCanvasPanelSlot>(AirplaneRouteImage->Slot);
	FVector2D NewSize = AirplaneRouteImageCanvasSlot->GetSize();
	NewSize.X = FVector2D::Distance(StartPos, DestPos);
	AirplaneRouteImageCanvasSlot->SetSize(NewSize);

}

FReply UC_MainMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return (HandleLMBDown(InGeometry, InMouseEvent) || HandleRMBDown(InGeometry, InMouseEvent)) ?
		FReply::Handled() : FReply::Unhandled();
}

FReply UC_MainMapWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bIsMouseDragging) return FReply::Unhandled();

	this->GetRenderTransform().Scale;

	FVector2D CurDragMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D DragDelta = CurDragMousePos - PrevDragMousePos;

	//UC_Util::Print(DragDelta);

	PrevDragMousePos = CurDragMousePos;

	//FVector2D MainMapPosition = MainMapImg->GetRenderTransform().Translation;
	//MainMapPosition += DragDelta;
	MainMapPosLerpDest += DragDelta;

	// Border check
	//float PosLimit		= (MainMapScale * CANVAS_SIZE - CANVAS_SIZE) * 0.5f;
	//MainMapPosition.X	= FMath::Clamp(MainMapPosition.X, -PosLimit, PosLimit);
	//MainMapPosition.Y	= FMath::Clamp(MainMapPosition.Y, -PosLimit, PosLimit);
	float PosLimit = (MainMapScaleLerpDest * CANVAS_SIZE - CANVAS_SIZE) * 0.5f;
	MainMapPosLerpDest.X = FMath::Clamp(MainMapPosLerpDest.X, -PosLimit, PosLimit);
	MainMapPosLerpDest.Y = FMath::Clamp(MainMapPosLerpDest.Y, -PosLimit, PosLimit);
	//MainMapImg->SetRenderTranslation(MainMapPosition);

	return FReply::Handled();
}

FReply UC_MainMapWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsMouseDragging = false;

	return FReply::Handled();
}

FReply UC_MainMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!MainMapImg)
	{
		UC_Util::Print("From UC_MainMapWidget::NativeOnMouseWheel : MainImg nullptr");
		return FReply::Unhandled();
	}

	float ScrollDelta = InMouseEvent.GetWheelDelta();

	// +�� ���� �ø�
	//MainMapScale += ScrollDelta * SCROLL_DELTA_STEP;
	//MainMapScale = FMath::Clamp(MainMapScale, 1.f, MAP_SCALE_MAX);
	//MainMapImg->SetRenderScale(FVector2D(MainMapScale, MainMapScale));

	if (ScrollDelta > 0.f && MainMapScaleLerpDest >= MAP_SCALE_MAX) return FReply::Handled();
	if (ScrollDelta < 0.f && MainMapScaleLerpDest <= 1.f) return FReply::Handled();

	MainMapScaleLerpDest += ScrollDelta * SCROLL_DELTA_STEP;
	MainMapScaleLerpDest = FMath::Clamp(MainMapScaleLerpDest, 1.f, MAP_SCALE_MAX);

	// ���콺 ������ ��ġ�� ���� Ȯ�� �κ� �̵� (Ȯ��ÿ��� �̵��� ����)
	if (ScrollDelta > 0.f)
	{
		FVector2D TargetPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		TargetPos.X = FMath::Clamp(TargetPos.X, MID_POINT.X - CANVAS_SIZE * 0.5f, MID_POINT.X + CANVAS_SIZE * 0.5f);
		FVector2D MidToMousePosDelta = MID_POINT - TargetPos;

		MainMapPosLerpDest += MidToMousePosDelta * (1 / MainMapScaleLerpDest);
	}

	// Border check
	//float PosLimit				= (MainMapScale * CANVAS_SIZE - CANVAS_SIZE) * 0.5f;
	//FVector2D MainMapPosition	= MainMapImg->GetRenderTransform().Translation;
	//MainMapPosition.X			= FMath::Clamp(MainMapPosition.X, -PosLimit, PosLimit);
	//MainMapPosition.Y			= FMath::Clamp(MainMapPosition.Y, -PosLimit, PosLimit);
	//MainMapImg->SetRenderTranslation(MainMapPosition);

	float PosLimit = (MainMapScaleLerpDest * CANVAS_SIZE - CANVAS_SIZE) * 0.5f;
	MainMapPosLerpDest.X = FMath::Clamp(MainMapPosLerpDest.X, -PosLimit, PosLimit);
	MainMapPosLerpDest.Y = FMath::Clamp(MainMapPosLerpDest.Y, -PosLimit, PosLimit);

	return FReply::Handled();
}

bool UC_MainMapWidget::SpawnPingImage(FVector WorldPingLocation)
{
	if (!PingMarkerBorder) return false;

	PingMarkerPos = GetWorldToMapSizePos(WorldPingLocation);
	PingMarkerBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	return true;
}

bool UC_MainMapWidget::SpawnPingImage(FVector2D MousePos)
{
	if (!PingMarkerBorder) return false;

	PingMarkerPos = MousePos - MID_POINT;
	PingMarkerPos -= MainMapImg->GetRenderTransform().Translation;
	PingMarkerPos /= MainMapScale;

	// Spawn MainMap Ping
	PingMarkerBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	FVector2D WorldPingPos2D = PingMarkerPos * (WORLD_MAP_SIZE / CANVAS_SIZE);

	// Landscape������ LineTrace ó��
	FCollisionQueryParams CollisionParams{};
	FHitResult HitResult{};

	static const float LINETRACE_START_HEIGHT = 25000.f;
	FVector LineTraceStartPos = { -WorldPingPos2D.Y, WorldPingPos2D.X, LINETRACE_START_HEIGHT };
	FVector LineTraceEndPos = LineTraceStartPos - FVector(0.f, 0.f, LINETRACE_START_HEIGHT);

	bool HasHit =
		GetWorld()->LineTraceSingleByChannel
		(
			HitResult,
			LineTraceStartPos,
			LineTraceEndPos,
			ECC_WorldStatic,
			CollisionParams
		);

	FVector WorldPingPos = (HasHit) ? HitResult.ImpactPoint :
		FVector(-WorldPingPos2D.Y, WorldPingPos2D.X, FIXED_LANDSCAPE_HEIGHT);

	// Spawn World ping
	OwnerPlayer->GetPingSystemComponent()->SpawnWorldPingActor(WorldPingPos);

	// Spawn MiniMap ping
	OwnerPlayer->GetHUDWidget()->GetMiniMapWidget()->SpawnPingImage(WorldPingPos);

	// Spawn Compass bar ping
	OwnerPlayer->GetHUDWidget()->SpawnCompassBarPingMarker(WorldPingPos);

	return true;
}

bool UC_MainMapWidget::CancelPingMarker()
{
	// World ping, MiniMap Ping ��� Hidden ó���ϱ�
	PingMarkerBorder->SetVisibility(ESlateVisibility::Hidden);

	if (!OwnerPlayer)
	{
		UC_Util::Print("From UC_MainMapWidget::CancelPingMarker : Player nullptr");
		return false;
	}

	OwnerPlayer->GetHUDWidget()->GetMiniMapWidget()->OnCancelPingMarker();
	OwnerPlayer->GetPingSystemComponent()->OnCancelWorldPing();
	OwnerPlayer->GetHUDWidget()->HideCompassBarPingMarker();

	return true;
}

void UC_MainMapWidget::UpdateAirplaneImagePosition(const FVector& AirplaneLocation)
{
	FVector2D Position = GetWorldToMapSizePos(AirplaneLocation);
	Position *= MainMapScale;
	Position += MainMapImg->GetRenderTransform().Translation;

	AirplaneImg->SetRenderTranslation(Position);

	FVector2D AirplaneImgScale = FVector2D(1.f, 1.f) * MainMapScale;
	AirplaneImg->SetRenderScale(AirplaneImgScale);
}

//void UC_MainMapWidget::TogglePlayerMarkerImageVisibility(bool Visible)
//{
//	ESlateVisibility ImageVisibility = Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
//	PlayerMarkerImg->SetVisibility(ImageVisibility);
//}

bool UC_MainMapWidget::HandleLMBDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// ���� ���콺 ��ư Ŭ�� ó��
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
		return false;

	if (!MainMapImg)
	{
		UC_Util::Print("From UC_MainMapWidget::NativeOnMouseButtonDown : MainImg nullptr!");
		return false;
	}


	FVector2D ScreenSpacePosition = InMouseEvent.GetScreenSpacePosition();
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	//UC_Util::Print(MousePos);

	bIsMouseDragging = true;
	PrevDragMousePos = MousePos;

	// ó�� �Ϸ� �� �Է��� �Һ��ϰ� ���ĵ��� �ʰ� ��
	return true;
}

bool UC_MainMapWidget::HandleRMBDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// ������ ���콺 ��ư Ŭ�� ó��
	if (InMouseEvent.GetEffectingButton() != EKeys::RightMouseButton) return false;

	FVector2D ScreenSpacePosition = InMouseEvent.GetScreenSpacePosition();
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	// Border ó��
	float BorderXBottom = MID_POINT.X - CANVAS_SIZE * 0.5f;
	float BorderXTop = MID_POINT.X + CANVAS_SIZE * 0.5f;

	if (MousePos.X < BorderXBottom || MousePos.X > BorderXTop) return false;

	// PingMarker�� Hidden�� �ƴ� �� PingMarker���� Input ó���ϸ� Marker ���ֱ�
	if (PingMarkerBorder->GetVisibility() != ESlateVisibility::Hidden)
	{

		FVector2D MarkedPos = MousePos - MID_POINT;
		MarkedPos -= MainMapImg->GetRenderTransform().Translation;
		MarkedPos /= MainMapScale;

		// ���� �ȿ��� Click ó�� �Ǿ���
		if (FMath::Abs(PingMarkerPos.X - MarkedPos.X) <= PING_BORDER_SIZE * 0.5f / MainMapScale &&
			FMath::Abs(PingMarkerPos.Y - MarkedPos.Y) <= PING_BORDER_SIZE * 0.5f / MainMapScale)
		{
			CancelPingMarker();
			return true;
		}
	}

	// ���ο� �� ���
	SpawnPingImage(MousePos);

	//UC_Util::Print(MousePos);

	return true;
}

FVector2D UC_MainMapWidget::GetWorldToMapSizePos(FVector GameWorldLocation)
{
	FVector2D Pos = { GameWorldLocation.Y, -GameWorldLocation.X };
	return Pos * (CANVAS_SIZE / WORLD_MAP_SIZE);
}

