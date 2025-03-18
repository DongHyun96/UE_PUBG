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

bool UC_TargetLocationSettingHelper::TrySetRandomTargetLocationAtMagneticCircle(const FMagneticCircle& Circle)
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
			UC_Util::Print("20 % ", FColor::MakeRandomColor(), 10.f);
			
			float XDir = FMath::RandRange(-1.f, 1.f);
			float YDir = FMath::RandRange(-1.f, 1.f);

			FVector2D RandomDirection = { XDir, YDir };
			RandomDirection.Normalize();
			
			float RandomScalar = Circle.Radius * FMath::FRand();

			RandomPosition = CirclePos + RandomDirection * RandomScalar;
		}
		else
		{
			UC_Util::Print("80 % ", FColor::MakeRandomColor(), 10.f);
			
			FVector2D Direction = OwnerPos - CirclePos;
			Direction.Normalize();

			float RandomScalar = Circle.Radius * FMath::RandRange(0.7f, 0.95f); // 중점으로부터 70 ~ 95% 지점
			Direction = Direction.GetRotated(FMath::RandRange(-20.f, 20.f)); // +-20도 반경으로 Random하게 방향 setting

			RandomPosition = CirclePos + Direction * RandomScalar;
		}

		PickedPositionTile = GAMESCENE_MANAGER->GetContainingTileCoordinate(RandomPosition);
		
	} while (UC_WaterTileCheckerComponent::IsWaterTileCoord(PickedPositionTile) || ++AvoidingWaterTryCount > 100);

	UC_Util::Print("Try avoiding water count : " + FString::FromInt(AvoidingWaterTryCount), FColor::Red, 10.f);
	
	TArray<FVector> ImpactPoints{}; // 상공에서 z와 평행한 위치로 LineTraceMulti한 ImpactPoint결과를 담을 배열
	bool TraceSucceeded = GetTraceMultiDownward(RandomPosition, ImpactPoints);

	if (!TraceSucceeded)
	{
		UC_Util::Print
		(
			"From UC_TargetLocationSettingHelper::TrySetRandomTargetLocationAtMagneticCircle : LineTrace Downward Failed!, seems wrong position set!",
			FColor::Red,
			10.f
		);
		return false;
	}

	// ImpactPoint에서 근처 NavMesh 조사해서 추리기
	TArray<FVector> PickedNavMeshLocations{};
	for (const FVector& ImpactPoint : ImpactPoints)
	{
		static const FVector Extent = {200.f, 200.f, 200.f}; // 2m 반경으로 조사 예정
		FVector ProjectedLocation{};

		if (FindNearestNavMeshAtLocation(ImpactPoint, Extent, ProjectedLocation))
		{
			PickedNavMeshLocations.Add(ProjectedLocation);
			
			// TODO : For Testing : 이 라인 지우기
			DrawDebugSphere(GetWorld(), ProjectedLocation, 20, 20, FColor::Red, true);
		}
	}

	// Random하게 Pick해서 Setting
	int RandomIndex = FMath::RandRange(0, PickedNavMeshLocations.Num() - 1);	
	OwnerBehaviorComponent->SetTargetLocation(PickedNavMeshLocations[RandomIndex]);

	// TODO : 일단은 자기장 줄어들기 시작할 때에 NextCircle 외부에 있는 Enemy들 한 번에 단순히 이동으로 처리 (추후, 자기장 대기시간에 미리 이동시킬
	// TODO : Enemy들 정해서 미리 이동시키고, 이동시킨 Enemy들 또 다시 TargetLocation 잡아서 이동 방지 시키기
	OwnerBehaviorComponent->SetServiceType(EServiceType::IDLE);
	OwnerBehaviorComponent->SetIdleTaskType(EIdleTaskType::BASIC_MOVETO);
	
	// TODO : For Testing : 이 라인 지우기
	DrawDebugSphere(GetWorld(), PickedNavMeshLocations[RandomIndex], 20, 20, FColor::Green, true);
	GAMESCENE_MANAGER->GetPlayer()->GetMainMapWidget()->SpawnSkyDivingStateDestinationImage(PickedNavMeshLocations[RandomIndex]);
	return true;
}

bool UC_TargetLocationSettingHelper::GetTraceMultiDownward(const FVector2D& TracePosition, TArray<FVector>& OutImpactPoints)
{
	static const float TRACE_START_Z = 60000.f;

	FVector Start = { TracePosition.X, TracePosition.Y, TRACE_START_Z };
	FVector End = {Start.X, Start.Y, 0.f};

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

