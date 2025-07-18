﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WeaponTutorialChecker.h"

#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/Gun/C_TutorialGoalWidget.h"
#include "TrainingLevel/TrainingShootingTarget/C_TrainingShootingTarget.h"
#include "TrainingLevel/Tutorial/C_TutorialManager.h"
#include "TrainingLevel/Tutorial/C_TutorialStageTriggerBox.h"
#include "TrainingLevel/Tutorial/TutorialWidget/C_TutorialWidget.h"
#include "Utility/C_Util.h"

UC_WeaponTutorialChecker::UC_WeaponTutorialChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
	TutorialStage = ETutorialStage::WeaponTutorial;
}

void UC_WeaponTutorialChecker::BeginPlay()
{
	Super::BeginPlay();

	// 무기와 탄환 획득하기
	GoalData.Add(FGoalData());
	GoalData[0].SubGoalsAchieved.Init(false, 2);

	// R 재장전하기
	GoalData.Add(FGoalData());
	
	// 사격장으로 이동하기
	GoalData.Add(FGoalData());

	// 스코프 모드에서 목표물 명중하기
	GoalData.Add(FGoalData());
	GoalData[3].SubGoalsAchieved.Init(false, 10);
}

void UC_WeaponTutorialChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_WeaponTutorialChecker::InitStage()
{
	Super::InitStage();

	AC_Gun::WeaponTutorialDelegate.BindUObject(this, &UC_WeaponTutorialChecker::OnGoalAchieved);
	AC_Item_Bullet::WeaponTutorialDelegate.BindUObject(this, &UC_WeaponTutorialChecker::OnGoalAchieved);
	GoalTriggerBoxes[2]->AreaArrivedDelegate.BindUObject(this, &UC_WeaponTutorialChecker::OnGoalAchieved); // 사격사로 도착 Delegate 구독
	GoalTriggerBoxes[2]->SetDelegateParam({2, -1});

	AC_TrainingShootingTarget::WeaponTutorialDelegate.BindUObject(this, &UC_WeaponTutorialChecker::OnGoalAchieved);
	AC_TrainingShootingTarget::SetWeaponTutorialHitCount(0);
	OwnerTutorialManager->GetTutorialWidget()->GetTutorialGoalWidget(TutorialStage)->UpdateNumberInfoOnGoalWidget(0);
	
	SubscribedDelegates.Add(AC_Gun::WeaponTutorialDelegate);
	SubscribedDelegates.Add(AC_Item_Bullet::WeaponTutorialDelegate);
	SubscribedDelegates.Add(GoalTriggerBoxes[2]->AreaArrivedDelegate);
	SubscribedDelegates.Add(AC_TrainingShootingTarget::WeaponTutorialDelegate);
}


