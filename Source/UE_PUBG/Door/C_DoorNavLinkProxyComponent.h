// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_DoorNavLinkProxyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_DoorNavLinkProxyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_DoorNavLinkProxyComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	/// <summary>
	/// SearchLocation에서 가장 가까운 NavMeshLocation 찾기 
	/// </summary>
	/// <param name="SearchLocation"></param>
	/// <param name="OutNavMeshLocation"></param>
	/// <returns> 찾을 수 없다면 return false </returns>
	bool FindNearestNavMeshPoint(const FVector& SearchLocation, FVector& OutNavMeshLocation);

private:

	class ANavLinkProxy* NavLink{};
};
