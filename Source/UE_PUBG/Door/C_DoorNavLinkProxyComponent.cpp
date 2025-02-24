// Fill out your copyright notice in the Description page of Project Settings.


#include "Door/C_DoorNavLinkProxyComponent.h"

#include "Navigation/NavLinkProxy.h"


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

	// TODO : 정확한 NavMesh 위치 파악 및 조정이 필요

	NavLink->PointLinks[0].Left.Y = -150.f;
	NavLink->PointLinks[0].Right.Y = 150.f;
	NavLink->SetActorHiddenInGame(false);
	NavLink->SetSmartLinkEnabled(true);
	NavLink->bSmartLinkIsRelevant = true;

	
}

void UC_DoorNavLinkProxyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
