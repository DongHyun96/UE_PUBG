// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_PingSystemComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Widget.h"
#include "Character/C_WorldPingActor.h"
#include "Character/C_Player.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MapWidget.h"
#include "HUD/C_MainMapWidget.h"
#include "Singleton/C_GameSceneManager.h"


#include "Utility/C_Util.h"

UC_PingSystemComponent::UC_PingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_PingSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters Param{};
	Param.Owner = OwnerPlayer;
	WorldPingActor = GetWorld()->SpawnActor<AC_WorldPingActor>(WorldPingActorClass, Param);
}


// Called every frame
void UC_PingSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_PingSystemComponent::OnMMBStarted()
{
	// LineTrace To world from player cam
	APlayerCameraManager* PCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector CamPos		= PCameraManager->GetCameraLocation();
	FVector CamForward	= PCameraManager->GetCameraRotation().Vector();

	FVector DestLocation = CamPos + WorldPingActor->GetWorldPingDistanceLimit() * CamForward;

	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActors(GAMESCENE_MANAGER->GetAllCharacterActors());
	
	FHitResult HitResult{};
	
	bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, CamPos, DestLocation, ECC_Visibility, CollisionParams);

	if (!HasHit) return;
	
	WorldPingActor->SpawnPingActorToWorld(HitResult.ImpactPoint);

	OwnerPlayer->GetHUDWidget()->GetMiniMapWidget()->SpawnPingImage(HitResult.ImpactPoint);
	OwnerPlayer->GetMainMapWidget()->SpawnPingImage(HitResult.ImpactPoint);
	OwnerPlayer->GetHUDWidget()->SpawnCompassBarPingMarker(HitResult.ImpactPoint);
}

void UC_PingSystemComponent::SpawnWorldPingActor(FVector SpawnLocation)
{
	WorldPingActor->SpawnPingActorToWorld(SpawnLocation);
}

void UC_PingSystemComponent::OnCancelWorldPing()
{
	WorldPingActor->HideWorldPing();
}



