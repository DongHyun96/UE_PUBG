// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_WorldPingActor.h"

#include "C_Enemy.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Singleton/C_GameSceneManager.h"
#include "Components/WidgetComponent.h"
#include "Character/C_Player.h"

#include "Utility/C_Util.h"

const float AC_WorldPingActor::WORLD_PING_DISTANCE_LIMIT = 50000.f;

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

bool AC_WorldPingActor::SpawnPingActorToWorld(FVector SpawnLocation)
{
	PingWidgetComponent->SetVisibility(true);

	this->SetActorLocation(SpawnLocation);

	// TODO : 이 라인 지우기 (For Testing)
	if (!GAMESCENE_MANAGER->GetEnemies().IsEmpty())
	{
		UC_BehaviorComponent* FirstEnemyBehaviorComponent = GAMESCENE_MANAGER->GetEnemies()[0]->GetController<AC_EnemyAIController>()->GetBehaviorComponent(); 
		FirstEnemyBehaviorComponent->SetBasicTargetLocation(SpawnLocation);
		FirstEnemyBehaviorComponent->SetServiceType(EServiceType::IDLE);
		FirstEnemyBehaviorComponent->SetIdleTaskType(EIdleTaskType::BASIC_MOVETO);
	}

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
	if (!IsValid(GAMESCENE_MANAGER->GetPlayer())) return;
	
	FVector PlayerLocation = GAMESCENE_MANAGER->GetPlayer()->GetActorLocation();

	float DistanceToPlayer = FVector::Distance(PlayerLocation, this->GetActorLocation());

	if (DistanceToPlayer > WORLD_PING_DISTANCE_LIMIT)
	{
		PingWidgetComponent->GetUserWidgetObject()->SetRenderScale(FVector2D::Zero());
		return;
	}
	
	// 10m 안팎이면 scale 1로 조정
	if (DistanceToPlayer <= 100.f)
	{
		PingWidgetComponent->GetUserWidgetObject()->SetRenderScale( {1.f, 1.f} );
		return;
	}

	float Scale = 1 - 0.00002f * DistanceToPlayer;

	PingWidgetComponent->GetUserWidgetObject()->SetRenderScale({ Scale, Scale });


}



