// Fill out your copyright notice in the Description page of Project Settings.


#include "Door/C_DoorNavLinkProxyComponent.h"

#include "NavigationSystem.h"
#include "NavLinkCustomComponent.h"
#include "Navigation/NavLinkProxy.h"

#include "NavigationSystem.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "NavMesh/RecastNavMesh.h"
#include "Singleton/C_GameSceneManager.h"

#include "Utility/C_Util.h"

UC_DoorNavLinkProxyComponent::UC_DoorNavLinkProxyComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UC_DoorNavLinkProxyComponent::BeginPlay()
{
	Super::BeginPlay();

	// GetOwner()->GetActorLocation();
	
	FVector DoorBottomMid = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 50.f;

	NavLink = GetWorld()->SpawnActor<ANavLinkProxy>(ANavLinkProxy::StaticClass(), DoorBottomMid, GetOwner()->GetActorRotation());

	NavLink->PointLinks[0].Left.Y = -150.f;
	NavLink->PointLinks[0].Right.Y = 150.f;
	NavLink->SetActorHiddenInGame(false);
	NavLink->SetSmartLinkEnabled(true);
	NavLink->bSmartLinkIsRelevant = true;

	/*if (UNavLinkCustomComponent* LinkComp = NavLink->GetSmartLinkComp())
	{
		bool Registered = LinkComp->IsRegistered();
		UC_Util::Print(Registered ? "REGISTERED" : "UNREGISTERED", FColor::MakeRandomColor(), 10.f);
	}*/
	
	if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		bool bIsRelevant = NavSystem->IsNavigationRelevant(NavLink);
		UC_Util::Print(bIsRelevant ? "RELEVANT" : "NOT RELEVANT", FColor::MakeRandomColor(), 10.f);
		
		if (ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavSystem->GetDefaultNavDataInstance())) NavMesh->RebuildAll();
		NavSystem->Build();
	}
	else UC_Util::Print("Nav System nullptr", FColor::MakeRandomColor(), 10.f);
	

}

void UC_DoorNavLinkProxyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
