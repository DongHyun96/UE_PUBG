// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MapWidget/C_MapWidget.h"

#include "Utility/C_Util.h"

#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Character/C_Player.h"

#include "Character/Component/SkyDivingComponent/C_SkyDivingComponent.h"

#include "HUD/C_HUDWidget.h"

#include "Components/Border.h"

void UC_MapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UC_GameInstance* GameInstance = Cast<UC_GameInstance>(GetGameInstance());
	CurrentSelectedMap = GameInstance->GetCurrentSelectedLevelType();

	if (CurrentSelectedMap == ELevelType::TrainingGround)
	{
		MapImage->SetBrushFromTexture(TrainingGroundMapImageTexture);
		GridImage->SetBrushFromTexture(TrainingGroundMapGridImgTexture);
	}
	else // ShantyTown
	{
		ShantyTownMapImgMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(ShantyTownMapMaterialInstance, this);
		MapImage->SetBrushFromMaterial(ShantyTownMapImgMaterialInstanceDynamic);
		GridImage->SetBrushFromTexture(ShantyTownMapGridImgTexture);
	}

	// 현재 Level에 따른 중앙점 위치 setting
	OriginLocation = OriginLocations[CurrentSelectedMap];
}

void UC_MapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePlayerMarkerRotation();

	//HandleUpdatePlaneRouteStartDest();
	if (AirplaneRouteLine->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) HandleUpdatePlaneRouteTransform();
}

void UC_MapWidget::SetMainCircleInfo(float PosU, float PosV, float UVRad)
{
	if (!ShantyTownMapImgMaterialInstanceDynamic)
	{
		UC_Util::Print("From UC_MapWidget::SetMainCircleInfo : Invalid ShantyTownMapImgMaterialInstanceDynamic!", FColor::Red, 10.f);
		return;
	}

	ShantyTownMapImgMaterialInstanceDynamic->SetVectorParameterValue(TEXT("MainCircleInfo"), FLinearColor(PosU, PosV, UVRad));
}

void UC_MapWidget::SetNextCircleInfo(float PosU, float PosV, float UVRad)
{
	if (!ShantyTownMapImgMaterialInstanceDynamic)
	{
		UC_Util::Print("From UC_MapWidget::SetNextCircleInfo : Invalid ShantyTownMapImgMaterialInstanceDynamic!", FColor::Red, 10.f);
		return;
	}
	ShantyTownMapImgMaterialInstanceDynamic->SetVectorParameterValue(TEXT("NextCircleInfo"), FLinearColor(PosU, PosV, UVRad));
}

void UC_MapWidget::SetAirplaneRoute(TPair<FVector, FVector> StartDest)
{
	// Map에 배치될 위치 지점 찾기
	AirplaneRouteStartPos = GetWorldToMapSizePos(StartDest.Key);
	AirplaneRouteDestPos = GetWorldToMapSizePos(StartDest.Value);

	// Dest triangle 회전 잡기
	if (AirplaneRouteStartPos.X - AirplaneRouteDestPos.X == 0.f)
	{
		if (AirplaneRouteStartPos.Y - AirplaneRouteDestPos.Y > 0.f)
			AirplaneDestTriangle->SetRenderTransformAngle(0.f);
		else AirplaneDestTriangle->SetRenderTransformAngle(180.f);
			
		return;
	}

	if (AirplaneRouteStartPos.Y - AirplaneRouteDestPos.Y == 0.f)
	{
		if (AirplaneRouteStartPos.X - AirplaneRouteDestPos.X > 0.f)
			AirplaneDestTriangle->SetRenderTransformAngle(90.f);
		else
			AirplaneDestTriangle->SetRenderTransformAngle(-90.f);

		return;
	}
	
	float DX = AirplaneRouteDestPos.X - AirplaneRouteStartPos.X;
	float DY = AirplaneRouteStartPos.Y - AirplaneRouteDestPos.Y;

	float Angle = FMath::RadiansToDegrees(FMath::Atan(DY / DX));
	Angle	    = (DX < 0.f) ? 270.f - Angle : 90.f - Angle;

	AirplaneDestTriangle->SetRenderTransformAngle(Angle);

	// Airplane 이미지 회전 잡기
	AirplaneImage->SetRenderTransformAngle(Angle - 90.f);

	// AirplaneRoute 이미지 회전 잡기
	AirplaneRouteLine->SetRenderTransformAngle(Angle - 90.f);

	// AirplaneRoute 이미지 크기 잡기
	// 부모 MapWidget에서는 크기를 한 번만 잡아주되 자식에서는(MainMapWidget) Tick에서 지속적으로 잡아주기
	if (!AirplaneRouteImageCanvasSlot) AirplaneRouteImageCanvasSlot = Cast<UCanvasPanelSlot>(AirplaneRouteLine->Slot);
	
	FVector2D NewSize	= AirplaneRouteImageCanvasSlot->GetSize();
	NewSize.X			= FVector2D::Distance(AirplaneRouteStartPos, AirplaneRouteDestPos);
	AirplaneRouteImageCanvasSlot->SetSize(NewSize);
}

void UC_MapWidget::ToggleAirplaneImageVisibility(bool Visible)
{
	const ESlateVisibility ImageVisibility = Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
	AirplaneImage->SetVisibility(ImageVisibility);
}

void UC_MapWidget::ToggleAirplaneRouteVisibility(bool Visible)
{
	const ESlateVisibility ImageVisibility = Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;

	AirplaneStartCircle->SetVisibility(ImageVisibility);
	AirplaneDestTriangle->SetVisibility(ImageVisibility);
	AirplaneRouteLine->SetVisibility(ImageVisibility);
}

void UC_MapWidget::UpdatePlayerMarkerRotation()
{
	if (!OwnerPlayer) return;

	if (!PlayerMarker)
	{
		UC_Util::Print("From UpdatePlayerMarkerRotation : PlayerMarkerImg nullptr");
		return;
	}
	
	// 회전 잡기
	bool OnAirplane = OwnerPlayer->GetMainState() == EMainState::SKYDIVING &&
		OwnerPlayer->GetSkyDivingComponent()->GetSkyDivingState() == ESkyDivingState::READY;

	float PlayerMarkerAngle = OnAirplane ? OwnerPlayer->GetController()->GetControlRotation().Yaw : OwnerPlayer->GetActorRotation().Yaw;

	PlayerMarker->SetRenderTransformAngle(PlayerMarkerAngle);
}


