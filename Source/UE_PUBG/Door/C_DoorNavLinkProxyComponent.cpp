// Fill out your copyright notice in the Description page of Project Settings.


#include "Door/C_DoorNavLinkProxyComponent.h"

#include "NavigationSystem.h"
#include "Navigation/NavLinkProxy.h"
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

	FActorSpawnParameters Param{};
	Param.Owner = this->GetOwner();
	
	NavLink = GetWorld()->SpawnActor<ANavLinkProxy>(ANavLinkProxy::StaticClass(), DoorBottomMid, GetOwner()->GetActorRotation(), Param);

	NavLink->PointLinks[0].Left.Y = -50.f;
	NavLink->PointLinks[0].Right.Y = 50.f;

	FVector LeftNavMeshLocation{};
	FVector RightNavMeshLocation{};
	FVector LeftWorldLocation	= NavLink->GetTransform().TransformPosition(NavLink->PointLinks[0].Left);
	FVector RightWorldLocation	= NavLink->GetTransform().TransformPosition(NavLink->PointLinks[0].Right);

	bool LeftFounded	= FindNearestNavMeshPoint(LeftWorldLocation, LeftNavMeshLocation);
	bool RightFounded	= FindNearestNavMeshPoint(RightWorldLocation, RightNavMeshLocation);

	if (LeftFounded)	NavLink->PointLinks[0].Left = NavLink->GetTransform().InverseTransformPosition(LeftNavMeshLocation);
	if (RightFounded)	NavLink->PointLinks[0].Right = NavLink->GetTransform().InverseTransformPosition(RightNavMeshLocation);
	
	NavLink->SetActorHiddenInGame(false);
	NavLink->SetSmartLinkEnabled(true);
	NavLink->bSmartLinkIsRelevant = true;
}

void UC_DoorNavLinkProxyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UC_DoorNavLinkProxyComponent::FindNearestNavMeshPoint(const FVector& SearchLocation, FVector& OutNavMeshLocation)
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSystem)
	{
		UC_Util::Print("From UC_DoorNavLinkProxyComponent::FindNearestNavMeshPoint : NavSystem nullptr", FColor::Red, 10.f);
		return false;
	}
	
	FNavLocation NavLocation{};

	bool Founded = NavSystem->ProjectPointToNavigation
	(
		SearchLocation,
		NavLocation,
		FVector(200, 200, 200)
	);

	if (!Founded) return false;

	OutNavMeshLocation = NavLocation.Location;
	return true;
}
