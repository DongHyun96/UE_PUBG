// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/C_MainMapWidget.h"
#include "HUD/C_HUDWidget.h"

#include "Singleton/C_GameSceneManager.h"
#include "Character/C_Player.h"
#include "Character/Component/C_PingSystemComponent.h"
#include "HUD/C_MapWidget.h"
#include "Utility/C_Util.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UC_MainMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
	bIsEnabled = true;
}

void UC_MainMapWidget::OnMKey()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// Toggle Visibility of MainMap and MiniMap
	if (this->Visibility == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapWidget()->SetVisibility(ESlateVisibility::Hidden);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapBorder()->SetVisibility(ESlateVisibility::Hidden);


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
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget()->GetMiniMapBorder()->SetVisibility(ESlateVisibility::Visible);

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());

		PC->SetIgnoreLookInput(false);  // ���콺 �̵��� ���� ī�޶� ȸ���� ����
		//PC->SetIgnoreMoveInput(false);  // ���콺 Ŭ���� ���� �������� ����
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

	if (!Player)
	{
		UC_Util::Print("From UC_MainMapWidget::HandleUpdateMarkers : Player member var nullptr.");
		return;
	}

	// ��ġ ���
	FVector2D PlayerPos			 = { Player->GetActorLocation().Y, -Player->GetActorLocation().X };
	FVector2D PlayerMarkerPos	 = PlayerPos * (CANVAS_SIZE / WORLD_MAP_SIZE) * MainMapScale;

	//PlayerMarkerPos += MainMapImg->GetRenderTransform().Translation * (1 / MainMapScale);
	PlayerMarkerPos += MainMapImg->GetRenderTransform().Translation;


	PlayerMarkerImg->SetRenderTranslation(PlayerMarkerPos);

	// ȸ�� ���
	PlayerMarkerImg->SetRenderTransformAngle(Player->GetActorRotation().Yaw);

	if (!PingMarkerBorder)
	{
		UC_Util::Print("From UC_MainMapWidget::HandleUpdateMarkers : PingMarkerBorder nullptr");
		return;
	}

	FVector2D PingMarkerToMainMapPos = PingMarkerPos * MainMapScale;
	PingMarkerToMainMapPos += MainMapImg->GetRenderTransform().Translation;

	PingMarkerBorder->SetRenderTranslation(PingMarkerToMainMapPos);
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

	PingMarkerPos  = { WorldPingLocation.Y, -WorldPingLocation.X };
	PingMarkerPos *= (CANVAS_SIZE / WORLD_MAP_SIZE);

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

	FVector2D WorldPingPos2D	= PingMarkerPos * (WORLD_MAP_SIZE / CANVAS_SIZE);

	// Landscape������ LineTrace ó��
	FCollisionQueryParams CollisionParams{};
	FHitResult HitResult{};

	static const float LINETRACE_START_HEIGHT = 25000.f;
	FVector LineTraceStartPos	= { -WorldPingPos2D.Y, WorldPingPos2D.X, LINETRACE_START_HEIGHT };
	FVector LineTraceEndPos		= LineTraceStartPos - FVector(0.f, 0.f, LINETRACE_START_HEIGHT);

	bool HasHit	= 
		GetWorld()->LineTraceSingleByChannel
		(
			HitResult,
			LineTraceStartPos,
			LineTraceEndPos,
			ECC_WorldStatic,
			CollisionParams
		);

	FVector WorldPingPos = (HasHit) ?	HitResult.ImpactPoint :
										FVector( -WorldPingPos2D.Y, WorldPingPos2D.X, FIXED_LANDSCAPE_HEIGHT);

	// Spawn World ping
	Player->GetPingSystemComponent()->SpawnWorldPingActor(WorldPingPos);

	// Spawn MiniMap ping
	Player->GetHUDWidget()->GetMiniMapWidget()->SpawnPingImage(WorldPingPos);

	// Spawn Compass bar ping
	Player->GetHUDWidget()->SpawnCompassBarPingMarker(WorldPingPos);

	return true;
}

bool UC_MainMapWidget::CancelPingMarker()
{
	// World ping, MiniMap Ping ��� Hidden ó���ϱ�
	PingMarkerBorder->SetVisibility(ESlateVisibility::Hidden);

	if (!Player)
	{
		UC_Util::Print("From UC_MainMapWidget::CancelPingMarker : Player nullptr");
		return false;
	}

	Player->GetHUDWidget()->GetMiniMapWidget()->OnCancelPingMarker();
	Player->GetPingSystemComponent()->OnCancelWorldPing();
	Player->GetHUDWidget()->HideCompassBarPingMarker();

	return true;
}

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
	float BorderXTop	= MID_POINT.X + CANVAS_SIZE * 0.5f;

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

	UC_Util::Print(MousePos);

	return true;
}

