// Fill out your copyright notice in the Description page of Project Settings.


#include "Airplane/C_Airplane.h"

#include "Singleton/C_GameSceneManager.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"

#include "Character/Component/C_SkyDivingComponent.h"

#include "Components/SceneComponent.h"

#include "Utility/C_Util.h"

// Sets default values
AC_Airplane::AC_Airplane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Airplane::BeginPlay()
{
	Super::BeginPlay();

	this->SetActorHiddenInGame(true);

	//SetActorRotation(FRotator())
	//FindComponentByNam
	//GetDefaultSubobjectByName()

	for (int i = 0; i < 4; i++)
	{
		FString PropNameStr			= TEXT("Prop" + FString::FromInt(i));
		FName PropName				= FName(*PropNameStr);
		UStaticMeshComponent* Prop	= Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(PropName));
		if (Prop) Props.Add(Prop);
	}
}

// Called every frame
void AC_Airplane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsFlying) return;

	CheckFlightFinished();
	Move(DeltaTime);
	UpdatePassengersPosition();
	UpdateProps(DeltaTime);
	UpdatePlayerMapHUD();

}

void AC_Airplane::SetFlightDirection(FVector InDirection)
{
	InDirection.Normalize();
	FlightDirection = InDirection;
}

void AC_Airplane::StartFlight()
{
	IsFlying = true;

	this->SetActorHiddenInGame(false);
	this->SetActorLocation(StartPosition);

	// 비행기 비행 방향으로 회전시키기
	this->SetActorRotation(FlightDirection.Rotation());

	// MainMap UI에 비행기 이미지 Visibility 토글
	if (!PlayerHUDWidget) PlayerHUDWidget = GAMESCENE_MANAGER->GetPlayer()->GetHUDWidget();
	PlayerHUDWidget->GetMainMapWidget()->ToggleAirplaneImageVisibility(true);

	// MainMap UI에 PlayerMarker 이미지 Visibility 토글
	PlayerHUDWidget->GetMainMapWidget()->TogglePlayerMarkerImageVisibility(false);
}

void AC_Airplane::Move(const float& DeltaTime)
{ 
	if (!IsFlying) return;
	AddActorWorldOffset(FlightDirection * FLIGHT_SPEED * DeltaTime, false);
}

void AC_Airplane::UpdatePlayerMapHUD()
{
	// TODO : UI 업데이트 시키기
	PlayerHUDWidget->GetMainMapWidget()->UpdateAirplaneImagePosition(this->GetActorLocation());
}

void AC_Airplane::CheckFlightFinished()
{
	// TODO : 비행 끝났는지 체크 (맵 반경에 아예 나갔는지 체크)
	// 끝났다면 -> IsFlying false | MainMap UI Toggle airplane visibility
}

void AC_Airplane::UpdatePassengersPosition()
{
	for (AC_BasicCharacter* Character : GAMESCENE_MANAGER->GetAllCharacters())
	{
		EMainState MainState		= Character->GetMainState();
		ESkyDivingState DivingState = Character->GetSkyDivingComponent()->GetSkyDivingState();

		if (MainState != EMainState::SKYDIVING || DivingState != ESkyDivingState::READY) continue;
		
		Character->SetActorLocation(this->GetActorLocation());
	}
}

void AC_Airplane::UpdateProps(const float& DeltaTime)
{
	if (!IsFlying) return;

	PropRotZValue += DeltaTime * PROP_ROT_SPEED;
	PropRotZValue = FMath::Fmod(PropRotZValue, 360.f);

	for (UStaticMeshComponent* Prop : Props)
		Prop->SetRelativeRotation(FRotator(0.f, 0.f, PropRotZValue));
}


