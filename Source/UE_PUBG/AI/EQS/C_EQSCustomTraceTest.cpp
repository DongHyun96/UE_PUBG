// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EQS/C_EQSCustomTraceTest.h"

#include "AI/C_BehaviorComponent.h"
#include "AI/C_EnemyAIController.h"
#include "Character/C_Enemy.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "Singleton/C_GameSceneManager.h"

void UC_EQSCustomTraceTest::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* DataOwner = QueryInstance.Owner.Get();
	BoolValue.BindData(DataOwner, QueryInstance.QueryID);
	TraceFromContext.BindData(DataOwner, QueryInstance.QueryID);
	ItemHeightOffset.BindData(DataOwner, QueryInstance.QueryID);
	ContextHeightOffset.BindData(DataOwner, QueryInstance.QueryID);

	bool bWantsHit = BoolValue.GetValue();
	bool bTraceToItem = TraceFromContext.GetValue();
	float ItemZ = ItemHeightOffset.GetValue();
	float ContextZ = ContextHeightOffset.GetValue();

	TArray<FVector> ContextLocations;
	if (!QueryInstance.PrepareContext(Context, ContextLocations))
	{
		return;
	}

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(EnvQueryTrace), TraceData.bTraceComplex);

	TArray<AActor*> IgnoredActors;
	if (QueryInstance.PrepareContext(Context, IgnoredActors))
	{
		TraceParams.AddIgnoredActors(IgnoredActors);
	}

	// 자기자신 및 모든 캐릭터들 제외 (TargetCharacter만 배제)
	AC_Enemy* Querier					= Cast<AC_Enemy>(DataOwner);
	AActor* TargetCharacter 			= Querier->GetEnemyAIController()->GetBehaviorComponent()->GetTargetCharacter();
	TArray<AActor*> AllCharacterActors	= GAMESCENE_MANAGER->GetAllCharacterActors();
	AllCharacterActors.Remove(TargetCharacter);
	
	TraceParams.AddIgnoredActors(AllCharacterActors);

	// Querier 자기자신 Ignore 처리
	/*AActor* QuerierActor = Cast<AActor>(DataOwner);
	TraceParams.AddIgnoredActor(QuerierActor);*/

	switch (TraceData.TraceShape)
	{
	case EEnvTraceShape::Line:
		if (bTraceToItem)
		{
			RunTraces<true, EEnvTraceShape::Line>(*this, ContextLocations, QueryInstance, ContextZ, ItemZ, TraceData, TestPurpose, FilterType, bWantsHit, TraceParams);
		}
		else
		{
			RunTraces<false, EEnvTraceShape::Line>(*this, ContextLocations, QueryInstance, ContextZ, ItemZ, TraceData, TestPurpose, FilterType, bWantsHit, TraceParams);
		}
		break;
	case EEnvTraceShape::Box:
		if (bTraceToItem)
		{
			RunTraces<true, EEnvTraceShape::Box>(*this, ContextLocations, QueryInstance, ContextZ, ItemZ, TraceData, TestPurpose, FilterType, bWantsHit, TraceParams);
		}
		else
		{
			RunTraces<false, EEnvTraceShape::Box>(*this, ContextLocations, QueryInstance, ContextZ, ItemZ, TraceData, TestPurpose, FilterType, bWantsHit, TraceParams);
		}
		break;
	case EEnvTraceShape::Sphere:
		if (bTraceToItem)
		{
			RunTraces<true, EEnvTraceShape::Sphere>(*this, ContextLocations, QueryInstance, ContextZ, ItemZ, TraceData, TestPurpose, FilterType, bWantsHit, TraceParams);
		}
		else
		{
			RunTraces<false, EEnvTraceShape::Sphere>(*this, ContextLocations, QueryInstance, ContextZ, ItemZ, TraceData, TestPurpose, FilterType, bWantsHit, TraceParams);
		}
		break;
	case EEnvTraceShape::Capsule:
		if (bTraceToItem)
		{
			RunTraces<true, EEnvTraceShape::Capsule>(*this, ContextLocations, QueryInstance, ContextZ, ItemZ, TraceData, TestPurpose, FilterType, bWantsHit, TraceParams);
		}
		else
		{
			RunTraces<false, EEnvTraceShape::Capsule>(*this, ContextLocations, QueryInstance, ContextZ, ItemZ, TraceData, TestPurpose, FilterType, bWantsHit, TraceParams);
		}
		break;
	}
}

template <bool bTraceToItem, EEnvTraceShape::Type Shape>
void UC_EQSCustomTraceTest::RunTraces
(
	const UEnvQueryTest_Trace& 		Query,
	const TArrayView<FVector>& 		ContextLocations,
	FEnvQueryInstance&				QueryInstance,
	float 							ContextZ,
	float 							ItemZ,
	const FEnvTraceData&			TraceData,
	EEnvTestPurpose::Type			TestPurpose,
	EEnvTestFilterType::Type		FilterType,
	bool							bWantsHit,
	const FCollisionQueryParams&	TraceParams
) const
{
	ECollisionChannel TraceCollisionChannel = ECC_WorldStatic;
	FCollisionResponseParams ResponseParams = FCollisionResponseParams::DefaultResponseParam;
	if (TraceData.TraceMode == EEnvQueryTrace::Type::GeometryByProfile)
	{
		if (!UCollisionProfile::GetChannelAndResponseParams(TraceData.TraceProfileName, TraceCollisionChannel, ResponseParams))
		{
			UE_VLOG_ALWAYS_UELOG(QueryInstance.Owner.Get(), LogEQS, Error, 
				TEXT("Unable to fetch collision channel and response from TraceProfileName %s, test %s for query %s will automatically fail"),
				*TraceData.TraceProfileName.ToString(),
				*Query.GetName(),
				*QueryInstance.QueryName);

			for (FEnvQueryInstance::ItemIterator It(&Query, QueryInstance); It; ++It)
			{
				It.SetScore(TestPurpose, FilterType, !bWantsHit, bWantsHit);
			}

			return;
		}
	}
	else if (TraceData.TraceMode == EEnvQueryTrace::Type::GeometryByChannel)
	{
		TraceCollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceData.TraceChannel);
	}

	FVector3f TraceExtent(TraceData.ExtentX, TraceData.ExtentY, TraceData.ExtentZ);

	for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ContextIndex++)
	{
		ContextLocations[ContextIndex].Z += ContextZ;
	}

	for (FEnvQueryInstance::ItemIterator It(&Query, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = Query.GetItemLocation(QueryInstance, It.GetIndex()) + FVector(0, 0, ItemZ);
		AActor* ItemActor = Query.GetItemActor(QueryInstance, It.GetIndex());

		FCollisionQueryParams PerItemTraceParams(TraceParams);
		PerItemTraceParams.AddIgnoredActor(ItemActor);

		for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ContextIndex++)
		{
			const bool bHit = TraceShapeWithDir<bTraceToItem, Shape>(ItemLocation, ContextLocations[ContextIndex], QueryInstance.World, TraceCollisionChannel, PerItemTraceParams, TraceExtent, ResponseParams);
			It.SetScore(TestPurpose, FilterType, bHit, bWantsHit);
		}
	}
}

template <bool bTraceToItem, EEnvTraceShape::Type Shape>
bool UC_EQSCustomTraceTest::TraceShapeWithDir
(
	const FVector& 					ItemPos,
	const FVector& 					ContextPos,
	UWorld*							World,
	ECollisionChannel				Channel,
	const FCollisionQueryParams&	TraceParams,
	const FVector3f&				Extent,
	const FCollisionResponseParams& ResponseParams
) const
{
	if constexpr (bTraceToItem)
	{
		return TraceShape<Shape>(ContextPos, ItemPos, World, Channel, TraceParams, Extent, ResponseParams);
	}
	else
	{
		return TraceShape<Shape>(ItemPos, ContextPos, World, Channel, TraceParams, Extent, ResponseParams);
	}
}

template <EEnvTraceShape::Type Shape>
bool UC_EQSCustomTraceTest::TraceShape
(
	const FVector& 					Start,
	const FVector& 					End,
	UWorld*							World,
	ECollisionChannel				Channel,
	const FCollisionQueryParams&	TraceParams,
	const FVector3f&				Extent,
	const FCollisionResponseParams& ResponseParams
) const
{
	if constexpr (Shape == EEnvTraceShape::Line)
	{
		return World->LineTraceTestByChannel(Start, End, Channel, TraceParams, ResponseParams);
	}
	else if constexpr (Shape == EEnvTraceShape::Box)
	{
		return World->SweepTestByChannel(Start, End, FQuat((End - Start).Rotation()), Channel, FCollisionShape::MakeBox(Extent), TraceParams, ResponseParams);
	}
	else if constexpr (Shape == EEnvTraceShape::Sphere)
	{
		return World->SweepTestByChannel(Start, End, FQuat::Identity, Channel, FCollisionShape::MakeSphere(Extent.X), TraceParams, ResponseParams);
	}
	else if constexpr (Shape == EEnvTraceShape::Capsule)
	{
		return World->SweepTestByChannel(Start, End, FQuat::Identity, Channel, FCollisionShape::MakeCapsule(Extent.X, Extent.Z), TraceParams, ResponseParams);
	}
	else
	{
		[] <bool cond = false>() { static_assert(cond, "Unsupported value of EEnvTraceShape received in TraceShape"); }(); // static_assert must be type-dependent to avoid "ill-formed" code
		return false;
	}
}

