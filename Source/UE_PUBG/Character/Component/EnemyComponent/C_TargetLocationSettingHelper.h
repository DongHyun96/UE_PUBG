// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_TargetLocationSettingHelper.generated.h"

struct FMagneticCircle;

/// <summary>
/// 상황 별로 BehaviorComponent의 TargetLocation setting을 처리 담당하는 Enemy ActorComponent 
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_TargetLocationSettingHelper : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UC_TargetLocationSettingHelper();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void SetOwnerEnemy(class AC_Enemy* InOwnerEnemy) { OwnerEnemy = InOwnerEnemy; }

	/// <summary>
	/// Random한 TargetLocation setting
	/// </summary>
	/// <param name="MaxRadius"> : OwnerEnemy의 위치에서 Random한 위치를 뽑되, 최대 허용 Random 거리 </param>
	/// <returns> : 지정할 수 없는 상황이라면 return false </returns>
	bool SetRandomTargetLocation(const float& MaxRadius);

	/// <summary>
	/// <para> MagneticCircle(NextCircle) 안쪽의 Random한 위치로 InCircle TargetLocation 지정 시도 </para>
	/// <para> 외곽 쪽 setting될 확률 비중을 더 높게 잡음 </para>
	/// </summary>
	/// <param name="Circle"></param>
	/// <returns> 이미 Circle안 쪽으로 위치해 있거나, Circle의 Radius가 invalid하다면 return false </returns>
	bool TrySetRandomInCircleTargetLocationAtMagneticCircle(const FMagneticCircle& Circle);

	/// <summary>
	/// MainCircle 안쪽의 Random한 위치로 Basic TargetLocation 지정 시도
	/// </summary>
	/// <param name="MaxRadius"> : OwnerEnemy의 위치에서 Random한 위치를 뽑되, 최대 허용 Random 거리 </param>
	/// <returns> : 지정할 수 없는 상황이라면 return false </returns>
	bool SetRandomBasicTargetLocationInsideMainCircle(const float& MaxRadius);
	
private:
	
	/// <summary>
	/// XYPosition 상공에서 LineTrace해서 걸린 Random한 NavMesh위의 지점 찾기
	/// </summary>
	/// <param name="XYPosition"> : Target XY Position </param>
	/// <param name="OutLocation"> : 결과 값 </param>
	/// <returns> : NavMesh가 하나도 걸리지 않았다면 return false </returns>
	bool PickRandomNavMeshLocationAtXYPos(const FVector2D& XYPosition, FVector& OutLocation);
	
	/// <summary>
	/// 상공에서 아래 방향으로 LineMultiTrace 조사
	/// </summary>
	/// <param name="TracePosition"> : MultiTrace할 XY좌표 위치 </param>
	/// <param name="OutImpactPoints"> : 발견된 ImpactPoints들 </param>
	/// <returns> : Hit되지 않았다면 return false </returns>
	bool GetTraceMultiDownward(const FVector2D& TracePosition, TArray<FVector>& OutImpactPoints);
	
	/// <summary>
	/// 해당 Location으로부터 가장 가까운 NavMesh가 존재하는지 조사 
	/// </summary>
	/// <param name="Location"> : 조사할 위치 지점 </param>
	/// <param name="Extent"> : 조사 반경 </param>
	/// <param name="ProjectedLocation"> : NavMesh위로 보정된 가장 가까운 위치 </param>
	/// <returns> : Extent반경에 NavMesh가 없다면 return false </returns>
	bool FindNearestNavMeshAtLocation(const FVector& Location, const FVector& Extent, FVector& ProjectedLocation);

private:

	class AC_Enemy*				OwnerEnemy{};
	class UC_BehaviorComponent* OwnerBehaviorComponent{};

	
};
