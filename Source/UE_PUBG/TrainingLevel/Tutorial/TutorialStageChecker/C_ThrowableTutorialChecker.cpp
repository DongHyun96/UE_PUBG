// Fill out your copyright notice in the Description page of Project Settings.


#include "C_ThrowableTutorialChecker.h"

#include "Item/Weapon/ThrowingWeapon/C_GrenadeExplode.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "TrainingLevel/Tutorial/C_TutorialManager.h"
#include "TrainingLevel/Tutorial/C_TutorialStageTriggerBox.h"


UC_ThrowableTutorialChecker::UC_ThrowableTutorialChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
	TutorialStage = ETutorialStage::ThrowableTutorial;
}

void UC_ThrowableTutorialChecker::BeginPlay()
{
	Super::BeginPlay();

	// 투척류 획득하기
	GoalData.Add(FGoalData());

	// 수류탄 연습장으로 이동하기
	GoalData.Add(FGoalData());

	// 수류탄 던지기
	GoalData.Add(FGoalData());
}


void UC_ThrowableTutorialChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_ThrowableTutorialChecker::InitStage()
{
	Super::InitStage();
	
	// Looting Delegate 구독
	AC_ThrowingWeapon::ThrowableTutorialDelegate.BindUObject(this, &UC_ThrowableTutorialChecker::OnGoalAchieved);

	// Throwable 테스트장 도착 Delegate 구독
	GoalTriggerBoxes[1]->AreaArrivedDelegate.BindUObject(this, &UC_ThrowableTutorialChecker::OnGoalAchieved);
	GoalTriggerBoxes[1]->SetDelegateParam({1, -1});

	// Grenade Explode 처리 구독
	AC_GrenadeExplode::ThrowableTutorialDelegate.BindUObject(this, &UC_ThrowableTutorialChecker::OnGoalAchieved);

	SubscribedDelegates.Add(AC_ThrowingWeapon::ThrowableTutorialDelegate);
	SubscribedDelegates.Add(GoalTriggerBoxes[1]->AreaArrivedDelegate);
	SubscribedDelegates.Add(AC_GrenadeExplode::ThrowableTutorialDelegate);
	
	
}

