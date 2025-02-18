// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/C_TimelineUtility.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveLinearColor.h"
#include "Utility/C_Util.h"


UC_TimelineUtility::UC_TimelineUtility()
{
	FString Name = GetName() + "_Timeline";
	Timeline = CreateDefaultSubobject<UTimelineComponent>(FName(Name));


}






void UC_TimelineUtility::InitializeTimelines
(
	AC_BasicCharacter* InOwnerCharacter, 
	const TCHAR* FilePath,
	ECurveType InType,
	FName InInterpFunction,
	FName InFinishedFunction,
	float InPlayRate,
	float InTimelineLength,
	bool InbLoop
)
{
	OwnerCharter = InOwnerCharacter;

	Timeline->SetNetAddressable();  // 리플리케이션 가능
	CurType = InType;

	switch (InType)
	{
	case ECurveType::CURVEFLOAT:
		CurveFloat = LoadObject<UCurveFloat>(nullptr, FilePath);
		SetTimeLinesCurve(InType, InInterpFunction, InFinishedFunction, InPlayRate, InTimelineLength, InbLoop);
		break;
	case ECurveType::CURVEVECTOR:
		CurveVector = LoadObject<UCurveVector>(nullptr, FilePath);
		//SetTimeLinesCurve(InType, InInterpFunction, InFinishedFunction, InPlayRate, InTimelineLength, InbLoop);
		break;
	case ECurveType::CURVELINEARCOLOR:
		CurveColor = LoadObject<UCurveLinearColor>(nullptr, FilePath);
		SetTimeLinesCurve(InType, InInterpFunction, InFinishedFunction, InPlayRate, InTimelineLength, InbLoop);
		break;
	case ECurveType::TYPE_MAX:
		break;
	default:
		break;
	}

}

void UC_TimelineUtility::SetTimeLinesCurve
(
	ECurveType InType,
	FName InInterpFunction,
	FName InFinishedFunction,
	float InPlayRate,
	float InTimelineLength,
	bool InbLoop
)
{

	AC_Player* OwnerPlayer = Cast<AC_Player>(OwnerCharter);
	
	switch (InType)
	{
	case ECurveType::CURVEFLOAT:
		if (Timeline && CurveFloat)
		{
			//CurveFloatForSwitchCamera->Get
			//UC_Util::Print("Success");
			InterpFunctionFloat.BindUFunction(OwnerPlayer, InInterpFunction);
			TimelineFinished.BindUFunction(OwnerPlayer, InFinishedFunction);
			Timeline->AddInterpFloat(CurveFloat, InterpFunctionFloat);
			Timeline->SetTimelineFinishedFunc(TimelineFinished);
			Timeline->SetPlayRate(InPlayRate);  // 재생 속도 설정
			Timeline->SetTimelineLength(InTimelineLength);
			Timeline->SetLooping(InbLoop);  // 반복하지 않도록 설정
			//UC_Util::Print("Timeline Length: " + FString::SanitizeFloat(CameraTransitionTimeline->GetTimelineLength()));
		}
		break;
	case ECurveType::CURVEVECTOR:
		if (Timeline && CurveVector)
		{
			//CurveFloatForSwitchCamera->Get
			//UC_Util::Print("Success");
			InterpFunctionVector.BindUFunction(OwnerPlayer, InInterpFunction);
			TimelineFinished.BindUFunction(OwnerPlayer, InFinishedFunction);
			Timeline->AddInterpVector(CurveVector, InterpFunctionVector);
			Timeline->SetTimelineFinishedFunc(TimelineFinished);
			Timeline->SetPlayRate(InPlayRate);  // 재생 속도 설정
			Timeline->SetTimelineLength(InTimelineLength);
			Timeline->SetLooping(InbLoop);  // 반복하지 않도록 설정
			//UC_Util::Print("Timeline Length: " + FString::SanitizeFloat(CameraTransitionTimeline->GetTimelineLength()));
		}
		break;
	case ECurveType::CURVELINEARCOLOR:
		if (Timeline && CurveColor)
		{
			//CurveFloatForSwitchCamera->Get
			//UC_Util::Print("Success");
			InterpFunctionColor.BindUFunction(OwnerPlayer, InInterpFunction);
			TimelineFinished.BindUFunction(OwnerPlayer, InFinishedFunction);
			Timeline->AddInterpLinearColor(CurveColor, InterpFunctionColor);
			Timeline->SetTimelineFinishedFunc(TimelineFinished);
			Timeline->SetPlayRate(InPlayRate);  // 재생 속도 설정
			Timeline->SetTimelineLength(InTimelineLength);
			Timeline->SetLooping(InbLoop);  // 반복하지 않도록 설정
			//UC_Util::Print("Timeline Length: " + FString::SanitizeFloat(CameraTransitionTimeline->GetTimelineLength()));
		}
		break;
	case ECurveType::TYPE_MAX:
		break;
	default:
		break;
	}
}

void UC_TimelineUtility::SetTimelinePlayRate(float InPlayrate)
{
	Timeline->SetPlayRate(InPlayrate);  // 재생 속도 설정
}

void UC_TimelineUtility::PlayFromStart()
{
	//if (CurveFloat)
	//{
	//	UC_Util::Print(Timeline->GetPlayRate(), FColor::Blue);
	//	UC_Util::Print(Timeline->GetTimelineLength(), FColor::Blue);
	//}

	Timeline->PlayFromStart();

}




