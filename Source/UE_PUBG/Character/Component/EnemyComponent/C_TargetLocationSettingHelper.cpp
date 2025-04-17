// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/EnemyComponent/C_TargetLocationSettingHelper.h"

#include "NavigationSystem.h"
#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "AI/Service/C_BTServiceIdle.h"
#include "Character/C_Enemy.h"
#include "Character/C_Player.h"
#include "HUD/C_MainMapWidget.h"
#include "MagneticField/C_MagneticFieldManager.h"
#include "MagneticField/C_WaterTileCheckerComponent.h"
#include "Singleton/C_GameSceneManager.h"
#include "Utility/C_Util.h"

UC_TargetLocationSettingHelper::UC_TargetLocationSettingHelper()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UC_TargetLocationSettingHelper::BeginPlay()
{
	Super::BeginPlay();
	OwnerBehaviorComponent = OwnerEnemy->GetEnemyAIController()->GetBehaviorComponent();
}

void UC_TargetLocationSettingHelper::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UC_TargetLocationSettingHelper::SetRandomTargetLocation(const float& MaxRadius)
{
	const FVector2D OwnerPos		= UC_Util::GetXY(OwnerEnemy->GetActorLocation());
	const FVector2D RandomDirection = FVector2D(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f)).GetSafeNormal();
	const float RandomScalar		= FMath::RandRange(0.f, MaxRadius);
	const FVector2D RandomPosition	= OwnerPos + RandomDirection * RandomScalar;

	// 상공에서 RayCasting하여 걸린 NavMesh 중 Random한 위치 뽑기
	FVector PickedNavMeshLocation{};
	if (!PickRandomNavMeshLocationAtXYPos(RandomPosition, PickedNavMeshLocation)) return false;

	OwnerBehaviorComponent->SetBasicTargetLocation(PickedNavMeshLocation);
	return true;
}

bool UC_TargetLocationSettingHelper::TrySetRandomInCircleTargetLocationAtMagneticCircle(const FMagneticCircle& Circle)
{
	if (Circle.Radius <= 0.f) return false;
	
	const FVector2D CirclePos = UC_Util::GetXY(Circle.MidLocation);
	const FVector2D OwnerPos = UC_Util::GetXY(OwnerEnemy->GetActorLocation());

	// 이미 원 안 쪽에 있을 때
	if (FVector2D::Distance(CirclePos, OwnerPos) < Circle.Radius) return false;

	FVector2D RandomPosition{};
	TPair<uint8, uint8> PickedPositionTile{};
	uint8 AvoidingWaterTryCount{};
	bool bSetToNearCurrentLocation = FMath::FRand() > 0.2f; // 80%의 확률로 외곽 쪽으로 가깝게 Random하게 잡기 
	
	do
	{
		if (!bSetToNearCurrentLocation) 
		{
			float XDir = FMath::RandRange(-1.f, 1.f);
			float YDir = FMath::RandRange(-1.f, 1.f);

			FVector2D RandomDirection = { XDir, YDir };
			RandomDirection.Normalize();
			
			float RandomScalar = Circle.Radius * FMath::FRand();

			RandomPosition = CirclePos + RandomDirection * RandomScalar;
		}
		else
		{
			FVector2D Direction = OwnerPos - CirclePos;
			Direction.Normalize();

			float RandomScalar = Circle.Radius * FMath::RandRange(0.7f, 0.95f); // 중점으로부터 70 ~ 95% 지점
			Direction = Direction.GetRotated(FMath::RandRange(-20.f, 20.f)); // +-20도 반경으로 Random하게 방향 setting

			RandomPosition = CirclePos + Direction * RandomScalar;
		}

		PickedPositionTile = GAMESCENE_MANAGER->GetContainingTileCoordinate(RandomPosition);
		
	} while (UC_WaterTileCheckerComponent::IsWaterTileCoord(PickedPositionTile) || ++AvoidingWaterTryCount > 100);

	// UC_Util::Print("Try avoiding water count : " + FString::FromInt(AvoidingWaterTryCount), FColor::Red, 10.f);

	FVector PickedNavMeshLocation{};
	bool Picked = PickRandomNavMeshLocationAtXYPos(RandomPosition, PickedNavMeshLocation);

	if (!Picked)
	{
		UC_Util::Print
		(
			"From UC_TargetLocationSettingHelper::TrySetRandomTargetLocationAtMagneticCircle : PickRandomNavMeshLocationAtXYPos failed!, seems wrong position set!",
			FColor::Red,
			10.f
		);
		return false;
	}
	
	OwnerBehaviorComponent->SetInCircleTargetLocation(PickedNavMeshLocation);

	// TODO : 일단은 자기장 줄어들기 시작할 때에 NextCircle 외부에 있는 Enemy들 한 번에 단순히 이동으로 처리 (추후, 자기장 대기시간에 미리 이동시킬
	// TODO : Enemy들 정해서 미리 이동시키고, 이동시킨 Enemy들 또 다시 TargetLocation 잡아서 이동 방지 시키기
	OwnerBehaviorComponent->SetServiceType(EServiceType::IDLE);
	OwnerBehaviorComponent->SetIdleTaskType(EIdleTaskType::INCIRCLE_MOVETO);
	
	// TODO : For Testing : 이 라인 지우기
	DrawDebugSphere(GetWorld(), PickedNavMeshLocation, 20, 20, FColor::Green, true);
	GAMESCENE_MANAGER->GetPlayer()->GetMainMapWidget()->SpawnSkyDivingStateDestinationImage(PickedNavMeshLocation);
	return true;
}

bool UC_TargetLocationSettingHelper::SetRandomBasicTargetLocationInsideMainCircle(const float& MaxRadius)
{
	if (!IsValid(GAMESCENE_MANAGER->GetMagneticFieldManager())) return false;
	
	FMagneticCircle MainCircle	= GAMESCENE_MANAGER->GetMagneticFieldManager()->GetMainCircle();
	const FVector2D CirclePos	= UC_Util::GetXY(MainCircle.MidLocation);
	const FVector2D OwnerPos	= UC_Util::GetXY(OwnerEnemy->GetActorLocation());

	// 2가지 -> MainCircle 내의 점으로 잡혀야 함 / 밑으로 Trace가 성공한 지점이어야 함

	FVector2D RandomPosition{};
	int TryCount{};
	do
	{
		const FVector2D RandomDirection = FVector2D(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f)).GetSafeNormal();
		const float RandomScalar = FMath::RandRange(0.f, MaxRadius);
		RandomPosition = OwnerPos + RandomDirection * RandomScalar;
	} while (FVector2D::Distance(CirclePos, RandomPosition) > MainCircle.Radius || ++TryCount > 50);

	// 제대로 된 위치를 못잡았다고 판단
	if (TryCount > 50) return false;

	// 상공에서 RayCasting하여 걸린 NavMesh 위치 중 Random한 위치 뽑기 
	FVector PickedNavMeshLocation{};
	if (!PickRandomNavMeshLocationAtXYPos(RandomPosition, PickedNavMeshLocation)) return false;

	OwnerBehaviorComponent->SetBasicTargetLocation(PickedNavMeshLocation);
	return true;
}

bool UC_TargetLocationSettingHelper::PickRandomNavMeshLocationAtXYPos(const FVector2D& XYPosition, FVector& OutLocation)
{
	TArray<FVector> ImpactPoints{};
	bool TraceSucceeded = GetTraceMultiDownward(XYPosition, ImpactPoints);

	if (!TraceSucceeded) return false;

	TArray<FVector> PickedNavMeshLocations{};

	for (const FVector& ImpactPoint : ImpactPoints)
	{
		static const FVector Extent = {200.f, 200.f, 200.f}; // 2m 반경으로 조사 예정
		FVector ProjectedLocation{};

		if (FindNearestNavMeshAtLocation(ImpactPoint, Extent, ProjectedLocation))
			PickedNavMeshLocations.Add(ProjectedLocation);
	}

	if (PickedNavMeshLocations.Num() <= 0) return false;

	// Random하게 Pick
	int RandomIndex = FMath::RandRange(0, PickedNavMeshLocations.Num() - 1);
	OutLocation = PickedNavMeshLocations[RandomIndex];	
	return true;
}

bool UC_TargetLocationSettingHelper::GetTraceMultiDownward(const FVector2D& TracePosition, TArray<FVector>& OutImpactPoints)
{
	static const float TRACE_START_Z = 60000.f;

	FVector Start = { TracePosition.X, TracePosition.Y, TRACE_START_Z };
	FVector End   = {Start.X, Start.Y, 0.f};

	FCollisionQueryParams CollisionParams{};
	CollisionParams.AddIgnoredActors(GAMESCENE_MANAGER->GetAllCharacterActors());

	TArray<FHitResult> OutHits{};

	bool HasHit = GetWorld()->LineTraceMultiByChannel(OutHits, Start, End, ECC_Visibility, CollisionParams);
	
	for (FHitResult& HitResult : OutHits)
		OutImpactPoints.Add(HitResult.ImpactPoint);
	
	return HasHit;
}

bool UC_TargetLocationSettingHelper::FindNearestNavMeshAtLocation(const FVector& Location, const FVector& Extent, FVector& ProjectedLocation)
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSystem)
	{
		UC_Util::Print("From UC_TargetLocationSettingHelper::FindNearestNavMeshAtLocation : NavSystem nullptr", FColor::Red, 10.f);
		return false;
	}
	
	FNavLocation NavLocation{};

	bool Founded = NavSystem->ProjectPointToNavigation
	(
		Location,
		NavLocation,
		Extent
	);

	if (!Founded) return false;

	ProjectedLocation = NavLocation.Location;
	return true;
}

