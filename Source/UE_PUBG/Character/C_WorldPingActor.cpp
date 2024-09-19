// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_WorldPingActor.h"

#include "Singleton/C_GameSceneManager.h"
#include "Components/WidgetComponent.h"
#include "Character/C_Player.h"

#include "Utility/C_Util.h"

AC_WorldPingActor::AC_WorldPingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	PingWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("PingWidgetComponent");
}

void AC_WorldPingActor::BeginPlay()
{
	Super::BeginPlay();

	PingWidgetComponent->SetVisibility(false);
}

void AC_WorldPingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleUpdateWorldPingScale();
}

bool AC_WorldPingActor::SpawnPingActorToWorld(FVector SpawnPos)
{
	PingWidgetComponent->SetVisibility(true);

	this->SetActorLocation(SpawnPos);

	return true;
}

void AC_WorldPingActor::HideWorldPing()
{
	PingWidgetComponent->SetVisibility(false);
}

void AC_WorldPingActor::HandleUpdateWorldPingScale()
{
	if (!PingWidgetComponent->GetUserWidgetObject())
	{
		UC_Util::Print("From AC_WorldPingActor::HandleUpdateWorldPingScale : PingWidget Component User widget nullptr!");
		return;
	}

	if (!PingWidgetComponent->IsVisible()) return;

	//PingWidgetComponent->GetUserWidgetObject()->SetRenderScale();
	FVector PlayerLocation = GAMESCENE_MANAGER->GetPlayer()->GetActorLocation();

	float DistanceToPlayer = FVector::Distance(PlayerLocation, this->GetActorLocation());

	if (DistanceToPlayer > WORLD_PING_DISTANCE_LIMIT)
	{
		PingWidgetComponent->GetUserWidgetObject()->SetRenderScale(FVector2D::Zero());
		return;
	}
	
	// 30m �����̸� scale 1�� ����
	if (DistanceToPlayer <= 300.f)
	{
		PingWidgetComponent->GetUserWidgetObject()->SetRenderScale( {1.f, 1.f} );
		return;
	}

	float Scale = 1 - 0.00002f * DistanceToPlayer;

	PingWidgetComponent->GetUserWidgetObject()->SetRenderScale({ Scale, Scale });


}

