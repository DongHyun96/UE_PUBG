// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Tests/EnvQueryTest_Trace.h"
#include "C_EQSCustomTraceTest.generated.h"

/**
 * 기존의 QueryTest_Trace에 Ignore Actor 수정이 불가능해서 RunTest 부분의 Ignore Actor 추가 부분만 수정
 * 기본적인 Test 구현은 모두 같음
 */
UCLASS()
class UE_PUBG_API UC_EQSCustomTraceTest : public UEnvQueryTest_Trace
{
	GENERATED_BODY()

protected:

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

private:
	/// <summary>
	/// UEnvQueryTest_Trace cpp의 namespace EnvQueryTest_Trace_Helpers 내부 함수 duplicate
	/// </summary>
	template <bool bTraceToItem, EEnvTraceShape::Type Shape>
	void RunTraces
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
	) const;

	/// <summary>
	/// UEnvQueryTest_Trace cpp의 namespace EnvQueryTest_Trace_Helpers 내부 함수 duplicate
	/// </summary>
	template <bool bTraceToItem, EEnvTraceShape::Type Shape>
	bool TraceShapeWithDir
	(
		const FVector& 					ItemPos,
		const FVector& 					ContextPos,
		UWorld*							World,
		enum ECollisionChannel			Channel,
		const FCollisionQueryParams&	TraceParams,
		const FVector3f&				Extent,
		const FCollisionResponseParams& ResponseParams
	) const;

	/// <summary>
	/// UEnvQueryTest_Trace cpp의 namespace EnvQueryTest_Trace_Helpers 내부 함수 duplicate
	/// </summary>
	template <EEnvTraceShape::Type Shape>
	bool TraceShape
	(
		const FVector& 					Start,
		const FVector& 					End,
		UWorld*							World,
		enum ECollisionChannel			Channel,
		const FCollisionQueryParams&	TraceParams,
		const FVector3f&				Extent,
		const FCollisionResponseParams& ResponseParams
	) const;
};
